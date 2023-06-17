#include "fs_sink.hpp"

#include <boost/regex.hpp>

#include <common/include/binary.hpp>
#include <common/include/logging.hpp>
#include <common/include/format.hpp>
#include <common/include/utils/algo.hpp>
#include <common/include/transactions.hpp>
#include <files/include/transaction.hpp>

#include <fs_sink/db_binary.hpp>
#include <models/exceptions.hpp>


namespace documents::fs_sink {

// Documents index (DocumentInfo + DocumentPosition) is stored to a separate file meta.ddb.
// Document payloads are stored to a number of data files. Indexed DocumentPosition
// provides information about the exact location of a specific DocumentPayload. 

namespace {

using TransactionGuard =
    common::transactions::TransactionGuard<files::transaction::FileTransaction>;

static const std::string kMetaFileName = "meta.ddb";
static const std::string kMetaPrefix = "META";
static const boost::regex kPageFileRegex("^page_\\d+\\.dp$");
static constexpr const size_t kMaxPageSize = 1024 * 1024 * 4; // 4Mb


std::filesystem::path GetIndexPath(std::filesystem::path path) {
    return path / kMetaFileName;
}

auto OpenMetaFileIn(const std::filesystem::path& path) {
    try {
        return common::binary::BinaryInStream(path);
    } catch (const std::ios_base::failure& ex) {
        throw exceptions::FilesystemException(common::format::Format(
            "Cannot open index file {}: {}",
            path, ex.what()));
    }
}

auto OpenMetaFileOut(const std::filesystem::path& path) {
    try {
        return common::binary::BinaryOutStream(path, true);
    } catch (const std::ios_base::failure& ex) {
        throw exceptions::FilesystemException(common::format::Format(
            "Cannot open index file {}: {}",
            path, ex.what()));
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
        LOG_DEBUG() << "Found data page " << page.Path() << " of size " << page.Size();
        files_map.insert({page.Index(), std::move(page)});
    }
    return files_map;
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

    auto stream = OpenMetaFileOut(meta_path_);
    TransactionGuard guard(meta_path_);
    stream << kMetaPrefix;
    stream << documents_info.size();
    for (const auto& [_, info] : documents_info) {
        stream << *info;
    }
    guard.Commit();

    LOG_DEBUG() << "Storing completed";
}

models::DocumentPosition FileStorageSink::Store(const std::optional<models::DocumentPosition>& old_position_opt,
                                                const models::DocumentPayloadPtr payload_ptr) {
    size_t actual_size = GetPayloadSize(payload_ptr);
    models::DocumentPosition new_position = FindAvailablePosition(actual_size);
    std::optional<size_t> old_offset =
        old_position_opt.has_value() ? std::make_optional(old_position_opt.value().page_offset) : std::nullopt;

    PageFile page(path_, new_position.page_index);
    {
        TransactionGuard page_guard(page.Path());
        page.StorePayload(payload_ptr, new_position.page_offset, old_offset);
        pages_map_.insert_or_assign(page.Index(), page);

        // disable old payload if it was on another page
        if (old_position_opt.has_value() && old_position_opt.value().page_index != new_position.page_index) {
            PageFile old_page(path_, old_position_opt.value().page_index);
            {
                TransactionGuard old_page_guard(old_page.Path());
                old_page.DisablePayload(old_position_opt.value().page_offset);
                old_page_guard.Commit();
            }
        }

        page_guard.Commit();
    }

    LOG_DEBUG() << "Payload is stored to " << new_position.page_index << ":" << new_position.page_offset;    
    return new_position;
}

void FileStorageSink::Delete(const models::DocumentPosition& position) {
    PageFile page(path_, position.page_index);
    TransactionGuard page_guard(page.Path());
    page.DisablePayload(position.page_offset);
    page_guard.Commit();
}

models::DocumentInfoMap FileStorageSink::LoadMeta() {
    LOG_INFO() << "Loading documents info from FS";

    try {
        auto stream = OpenMetaFileIn(meta_path_);

        std::string prefix{};
        stream >> prefix;
        if (prefix.empty() && stream.Eof()) {
            LOG_DEBUG() << "Meta data is empty";
            return models::DocumentInfoMap{};
        }

        if (prefix != kMetaPrefix) {
            LOG_ERROR() << common::format::Format("Invalid meta data prefix: {}", prefix);
            throw exceptions::FilesystemException();
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
    } catch (const std::ios_base::failure& ex) {
        LOG_WARNING() << "Meta file read error: " << ex.what();
        throw exceptions::FilesystemException();
    }
}

models::DocumentPayloadPtr FileStorageSink::LoadPayload(const models::DocumentPosition& position) {
    PageFile page(path_, position.page_index);
    return page.LoadPayload(position.page_offset);
}

void FileStorageSink::InitFs() {
    LOG_INFO() << "Init document DB file system storage at " << meta_path_;

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

models::DocumentPosition FileStorageSink::FindAvailablePosition(size_t size) {
    for (const auto& [index, page] : pages_map_) {
        if (page.Size() + size <= kMaxPageSize) {
            return models::DocumentPosition{
                index,        // page_index
                page.Size(),  // page_offset
            }; 
        }
    }

    const auto new_index = ++page_index_counter_;
    const auto new_offset = PageFile::GetDefaultPageSize();
    LOG_DEBUG() << "No suitable page found, creating new with index " << new_index;
    return models::DocumentPosition{
        new_index,   // page_index
        new_offset,  // page_offset
    };
}

void FileStorageSink::Swap(FileStorageSink&& other) {
    std::swap(path_, other.path_);
    std::swap(meta_path_, other.meta_path_);
}

} // namespace documents::fs_sink
