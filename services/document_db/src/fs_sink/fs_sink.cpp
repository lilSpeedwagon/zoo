#include "fs_sink.hpp"

#include <exception>
#include <filesystem>
#include <fstream>

#include <boost/regex.hpp>

#include <common/include/binary.hpp>
#include <common/include/logging.hpp>
#include <common/include/format.hpp>

#include <fs_sink/db_binary.hpp>
#include <models/exceptions.hpp>


namespace documents::fs_sink {

// Documents index (DocumentInfo + DocumentPosition) is stored to a separate file meta.ddb.
// Document payloads are stored to a number of data files. Indexed DocumentPosition
// provides information about the exact location of a specific DocumentPayload. 

namespace {

static const std::ios_base::openmode kFileWriteMode = std::ios::binary | std::ios::out;
static const std::ios_base::openmode kFileReadMode = std::ios::binary | std::ios::in;
static const std::string kMetaFileName = "meta.ddb";
static const std::string kMetaPrefix = "META";
static const std::string kPagePrefix = "PAGE";
static const std::string kPageFilePrefix = "page_";
static const std::string kPageFileExtension = ".dp";
static const boost::regex kPageFileRegex("^page_\\d+\\.dp$");
static constexpr const size_t kMaxPageSize = 1024 * 1024 * 4; // 4Mb
static constexpr const size_t kPayloadPrefixSize = 
    sizeof(bool) + sizeof(decltype(models::DocumentPayload().GetUnderlying().size()));

static const std::string kFilesystemErrorMsg = "Corrupted files found while loading database";


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

bool IsValidPage(const std::filesystem::path& path) {
    std::fstream file(path, kFileReadMode);
    std::string buffer;
    file.read(buffer.data(), kPagePrefix.size());
    return buffer != kPageFilePrefix;
}

size_t GetPageIndex(const std::filesystem::path& path) {
    const auto stem = path.stem().string();
    try {
        return std::stol(stem.substr(kPagePrefix.size() + 1));
    } catch (const std::logic_error& ex) {
        LOG_ERROR() << common::format::Format(
            "Cannot obtain page index for file {}: {}", path.string(), ex.what());
        throw exceptions::FilesystemException(kFilesystemErrorMsg);
    }
}

size_t GetPageSize(const std::filesystem::path& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error& ex) {
        LOG_ERROR() << common::format::Format(
            "Cannot obtain page size for file {}: {}", path.string(), ex.what());
        throw exceptions::FilesystemException(kFilesystemErrorMsg);
    }
}

std::filesystem::path GetPagePath(const std::filesystem::path& path, size_t page_index) {
    return path / std::filesystem::path(common::format::Format(
        "{}{}{}", kPageFilePrefix, page_index, kPageFileExtension));
}

std::unordered_map<size_t, FileStorageSink::PageFile> LoadPageFilesMap(const std::filesystem::path& path) {
    LOG_DEBUG() << "Analyzing data pages at " << path.string();
    std::unordered_map<size_t, FileStorageSink::PageFile> files_map;
    for (auto& item : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(item) ||
            !boost::regex_match(item.path().filename().string(), kPageFileRegex)) {
            continue;
        }

        if (!IsValidPage(item.path())) {
            LOG_ERROR() << item.path().string() << " file is corrupted";
            throw exceptions::FilesystemException(kFilesystemErrorMsg);
        }

        const auto index = GetPageIndex(item.path());
        FileStorageSink::PageFile page{
            std::move(item.path()),    // path
            GetPageSize(item.path()),  // size
        };
        LOG_DEBUG() << "Found data page " << page.path.string() << " of size " << page.size;
        files_map[index] = std::move(page);
    }
    return files_map;
}

std::optional<models::DocumentPosition> FindAvailablePosition(
    const std::unordered_map<size_t, FileStorageSink::PageFile>& pages_map, size_t size) {
    for (const auto& [index, page] : pages_map) {
        if (page.size + size <= kMaxPageSize) {
            return models::DocumentPosition{
                index,      // page_index
                page.size,  // page_offset
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
    bool is_new_page = false;
    size_t actual_size = kPayloadPrefixSize + payload_ptr->GetUnderlying().size();
    std::optional<models::DocumentPosition> new_position_opt = FindAvailablePosition(pages_map_, actual_size);
    if (!new_position_opt.has_value()) {
        is_new_page = true;
        const auto new_index = ++page_index_counter_;
        new_position_opt = models::DocumentPosition{
            new_index,           // page_index
            kPagePrefix.size(),  // page_offset
        };
        LOG_DEBUG() << "No suitable page found, creating new with index " << new_index;
    }

    auto& new_position = new_position_opt.value();
    auto page_file = WritePayload(new_position, is_new_page, actual_size, payload_ptr);

    // disable old payload
    if (old_position_opt.has_value()) {
        const bool old_pos_is_active = false;
        if (old_position_opt.value().page_index == new_position.page_index) {
            page_file.Seek(old_position_opt.value().page_offset);
            page_file << old_pos_is_active;
        } else {
            const auto old_file_path = GetPagePath(path_, old_position_opt.value().page_index);
            common::binary::BinaryOutStream old_file(old_file_path);
            old_file.Seek(old_position_opt.value().page_offset);
            old_file << old_pos_is_active;
        }

        // TODO old file cleanup
        // need to store number of active file for page
        // inc / dec on every payload storage
        // read all files on sync (only flags)
    }

    LOG_DEBUG() << "Payload is stored to " << new_position.page_index << ":" << new_position.page_offset;    
    return new_position;
}

void FileStorageSink::Delete(const models::DocumentPosition& position) {
    const auto path = GetPagePath(path_, position.page_index);
    common::binary::BinaryOutStream file(path);

    const bool is_active = false;
    file.Seek(position.page_offset);
    file << is_active;
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
    auto page_path = GetPagePath(path_, position.page_index);
    common::binary::BinaryInStream file(page_path);
    file.Seek(position.page_offset);
    bool is_active{};
    file >> is_active;
    if (!is_active) {
        LOG_ERROR() << "Document info points to an unactive payload: "
                    << position.page_index << ":" << position.page_offset;
        throw std::runtime_error("Corrupted data page found.");
    }
    models::DocumentPayload payload;
    file >> payload;
    return std::make_shared<models::DocumentPayload>(std::move(payload));
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

common::binary::BinaryOutStream FileStorageSink::WritePayload(
    const models::DocumentPosition& position, bool is_new_page, size_t payload_size,
    models::DocumentPayloadPtr payload_ptr) { 

    const auto path = GetPagePath(path_, position.page_index);
    auto size_diff = payload_size;
    common::binary::BinaryOutStream file(path);
    if (is_new_page) {
        file << kPagePrefix;
        size_diff += kPagePrefix.size();
        pages_map_[position.page_index] = PageFile{
            path,       // path
            size_diff,  // size
        };
    } else {
        pages_map_[position.page_index].size += size_diff;
    }

    const bool is_active = true;
    file.Seek(position.page_offset);
    file << is_active;
    file << *payload_ptr;

    return file;
}

void FileStorageSink::Swap(FileStorageSink&& other) {
    std::swap(path_, other.path_);
    std::swap(meta_path_, other.meta_path_);
}

} // namespace documents::fs_sink
