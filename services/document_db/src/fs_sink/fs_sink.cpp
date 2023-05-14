#include "fs_sink.hpp"

#include <exception>
#include <filesystem>
#include <fstream>

#include <boost/regex.hpp>

#include <common/include/binary.hpp>
#include <common/include/logging.hpp>
#include <common/include/format.hpp>
#include <common/include/utils/algo.hpp>

#include <fs_sink/db_binary.hpp>
#include <models/exceptions.hpp>


namespace documents::fs_sink {

// Documents index (DocumentInfo + DocumentPosition) is stored to a separate file meta.ddb.
// Document payloads are stored to a number of data files. Indexed DocumentPosition
// provides information about the exact location of a specific DocumentPayload. 

namespace {

static const std::string kMetaFileName = "meta.ddb";
static const std::string kMetaPrefix = "META";
static const std::string kPagePrefix = "PAGE";
static const boost::regex kPageFileRegex("^page_\\d+\\.dp$");
static constexpr const size_t kMaxPageSize = 1024 * 1024 * 4; // 4Mb


std::filesystem::path GetIndexPath(std::filesystem::path path) {
    path += "/meta.ddb";
    return path;
}

auto OpenMetaFileIn(const std::filesystem::path& path) {
    try {
        return common::binary::BinaryInStream(path);
    } catch (const std::runtime_error& ex) {
        throw std::runtime_error(common::format::Format(
            "Cannot open document_db index file {}: {}",
            path.string(), ex.what()));
    }
}

auto OpenMetaFileOut(const std::filesystem::path& path) {
    try {
        return common::binary::BinaryOutStream(path, true);
    } catch (const std::runtime_error& ex) {
        throw std::runtime_error(common::format::Format(
            "Cannot open document_db index file {}: {}",
            path.string(), ex.what()));
    }
}

size_t GetPayloadSize(models::DocumentPayloadPtr payload) {
    return sizeof(bool) + sizeof(size_t) + payload->GetUnderlying().size();  // move to binary size traits
}

std::unordered_map<size_t, PageFile> LoadPageFilesMap(const std::filesystem::path& path) {
    LOG_DEBUG() << "Analyzing data pages at " << path.string();
    std::unordered_map<size_t, PageFile> files_map;
    for (auto& item : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(item) ||
            !boost::regex_match(item.path().filename().string(), kPageFileRegex)) {
            continue;
        }

        PageFile page(item.path());
        LOG_DEBUG() << "Found data page " << page.Path().string() << " of size " << page.Size();
        files_map.insert({page.Index(), std::move(page)});
    }
    return files_map;
}

std::optional<models::DocumentPosition> FindAvailablePosition(
    const std::unordered_map<size_t, PageFile>& pages_map, size_t size) {
    for (const auto& [index, page] : pages_map) {
        if (page.Size() + size <= kMaxPageSize) {
            return models::DocumentPosition{
                index,        // page_index
                page.Size(),  // page_offset
            }; 
        }
    }
    return std::nullopt;
}

} // namespace

FileStorageSink::FileStorageSink(const std::filesystem::path& path) 
    : path_(path), meta_path_(GetIndexPath(path_)), page_index_counter_(0), pages_map_() {}

FileStorageSink::~FileStorageSink() {}

FileStorageSink::FileStorageSink(FileStorageSink&& other) {
    Swap(std::move(other));
}

FileStorageSink& FileStorageSink::operator=(FileStorageSink&& other) {
    Swap(std::move(other));
    return *this;
}

void FileStorageSink::Init() {
    InitFs();
}

void FileStorageSink::Reset() {
    pages_map_.clear();
    page_index_counter_ = 0;
    InitFs();
}
    
void FileStorageSink::Store(const models::DocumentInfoMap& documents_info) {
    LOG_DEBUG() << "Storing updated documents info to FS";

    // TODO acquire FS mutex
    // TODO start FS transaction

    auto stream = OpenMetaFileOut(meta_path_);
    stream << kMetaPrefix;
    stream << documents_info.size();
    for (const auto& [_, info] : documents_info) {
        stream << *info;
    }

    // TODO commit/rollback FS transaction
    // TODO release FS mutex

    LOG_DEBUG() << "Storing completed";
}

models::DocumentPosition FileStorageSink::Store(const std::optional<models::DocumentPosition>& old_position_opt,
                                                const models::DocumentPayloadPtr payload_ptr) {
    size_t actual_size = GetPayloadSize(payload_ptr);
    std::optional<models::DocumentPosition> new_position_opt = FindAvailablePosition(pages_map_, actual_size);
    if (!new_position_opt.has_value()) {
        const auto new_index = ++page_index_counter_;
        const auto new_offset = PageFile::GetDefaultPageSize();
        new_position_opt = models::DocumentPosition{
            new_index,   // page_index
            new_offset,  // page_offset
        };
        LOG_DEBUG() << "No suitable page found, creating new with index " << new_index;
    }

    auto& new_position = new_position_opt.value();
    std::optional<size_t> old_offset =
        old_position_opt.has_value() ? std::make_optional(old_position_opt.value().page_offset) : std::nullopt;

    PageFile page(path_, new_position.page_index);
    page.StorePayload(payload_ptr, new_position.page_offset, old_offset);
    pages_map_.insert_or_assign(page.Index(), page);

    // disable old payload if it was on another page
    if (old_position_opt.has_value() && old_position_opt.value().page_index == new_position.page_index) {
        PageFile old_page(path_, old_position_opt.value().page_index);
        old_page.DisablePayload(old_position_opt.value().page_offset);
    }

    LOG_DEBUG() << "Payload is stored to " << new_position.page_index << ":" << new_position.page_offset;    
    return new_position;
}

void FileStorageSink::Delete(const models::DocumentPosition& position) {
    PageFile page(path_, position.page_index);
    page.DisablePayload(position.page_offset);
}

models::DocumentInfoMap FileStorageSink::LoadMeta() {
    LOG_INFO() << "Loading documents info from FS";
    auto stream = OpenMetaFileIn(meta_path_);

    std::string prefix{};
    try {
        stream >> prefix;
    } catch (const std::runtime_error& ex) {
        LOG_WARNING() << "Prefix read error: " << ex.what();
    }
    if (prefix.empty() && stream.Eof()) {
        LOG_DEBUG() << "Meta data is empty";
        return models::DocumentInfoMap{};
    }

    if (prefix != kMetaPrefix) {
        throw std::runtime_error(
            common::format::Format("Invalid meta data prefix: {}", prefix));
    }

    size_t count{};
    stream >> count;
    LOG_DEBUG() << count << " document info entries found";
    models::DocumentInfoMap result{};
    for (size_t i = 0; i < count; i++) {
        models::DocumentInfo document_info{};
        stream >> document_info;
        const auto id = document_info.id;
        result[id] = std::make_shared<models::DocumentInfo>(std::move(document_info));
    }
    return result;
}

models::DocumentPayloadPtr FileStorageSink::LoadPayload(const models::DocumentPosition& position) {
    PageFile page(path_, position.page_index);
    return page.LoadPayload(position.page_offset);
}

void FileStorageSink::InitFs() {
    LOG_INFO() << "Init document DB file system storage at "
               << meta_path_.generic_string();

    const auto is_index_exists = std::filesystem::exists(meta_path_);
    if (!is_index_exists) {
        Store({});  // touch index file
    }

    pages_map_ = LoadPageFilesMap(path_);
    for (const auto& [index, _] : pages_map_) {
        if (index > page_index_counter_) {
            page_index_counter_ = index;
        }
    }
}

void FileStorageSink::Swap(FileStorageSink&& other) {
    std::swap(path_, other.path_);
    std::swap(meta_path_, other.meta_path_);
}

} // namespace documents::fs_sink
