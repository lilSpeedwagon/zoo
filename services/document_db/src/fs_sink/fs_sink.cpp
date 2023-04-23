#include "fs_sink.hpp"

#include <exception>
#include <filesystem>
#include <fstream>

#include <boost/regex.hpp>

#include <common/include/binary.hpp>
#include <common/include/logging.hpp>
#include <common/include/format.hpp>
#include <common/include/utils/errors.hpp>

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
static const boost::regex kPageFileRegex("^page_\d+\.dp$");
static constexpr const size_t kMaxPageSize = 1024 * 1024 * 4; // 4Mb

static const std::string kFilesystemErrorMsg = "Corrupted files found while loading database";


std::filesystem::path GetIndexPath(std::filesystem::path path) {
    path += "/meta.ddb";
    return path;
}

template<typename T>
void CheckStream(const T& stream) {
    if (!stream.is_open() || !stream.good()) {
        throw std::runtime_error(common::format::Format(
            "Cannot open document_db index file: {}",
            common::utils::errors::GetLastError()));
    }
}

auto OpenMetaFileIn(const std::filesystem::path& path) {
    try {
        std::ifstream stream(path, kFileReadMode);
        return common::binary::BinaryInStream(std::move(stream));
    } catch (const std::runtime_error& ex) {
        throw std::runtime_error(common::format::Format(
            "Cannot open document_db index file {}: {}",
            path.string(), ex.what()));
    }
}

auto OpenMetaFileOut(const std::filesystem::path& path) {
    try {
        std::ofstream stream(path, kFileWriteMode);
        return common::binary::BinaryOutStream(std::move(stream));
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
    size_t from = kPagePrefix.size();
    size_t to = stem.find('.', from);
    if (to == std::string::npos) {
        LOG_ERROR() << "Cannot obtain page index for file: " << path.string();
        throw exceptions::FilesystemException(kFilesystemErrorMsg);
    }

    try {
        return std::stol(stem.substr(from, to));
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

std::unordered_map<size_t, FileStorageSink::PageFile> LoadPageFilesMap(const std::filesystem::path& path) {
    std::unordered_map<size_t, FileStorageSink::PageFile> files_map;
    for (auto& item : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(item) &&
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
        files_map[index] = std::move(page);
    }
    return files_map;
}

} // namespace

FileStorageSink::FileStorageSink(const std::filesystem::path& path) 
    : path_(path), meta_path_(GetIndexPath(path_)), page_index_counter_(0), pages_map_() {
    InitFs();
}

FileStorageSink::~FileStorageSink() {}

FileStorageSink::FileStorageSink(FileStorageSink&& other) {
    Swap(std::move(other));
}

FileStorageSink& FileStorageSink::operator=(FileStorageSink&& other) {
    Swap(std::move(other));
    return *this;
}
    
void FileStorageSink::Store(const models::DocumentInfoMap& documents_info_) {
    LOG_INFO() << "Storing updated documents info to FS";

    // TODO acquire FS mutex
    // TODO start FS transaction

    auto stream = OpenMetaFileOut(meta_path_);
    stream << kMetaPrefix;
    stream << documents_info_.size();
    for (const auto& [_, info] : documents_info_) {
        stream << *info;
    }

    // TODO commit/rollback FS transaction
    // TODO release FS mutex

    LOG_INFO() << "Storing completed";
}

models::DocumentPosition FileStorageSink::Store(const models::DocumentPosition& old_position,
                                                const models::DocumentPayloadPtr payload_ptr) {
    LOG_DEBUG() << "Storing payload to " << old_position.page_index << ':' << old_position.page_offset;

    // TODO refactor to functions
    std::optional<models::DocumentPosition> new_pos = std::nullopt;
    size_t actual_size = payload_ptr->GetUnderlying().size() + sizeof(size_t); // move to some constexpr
    for (const auto& [index, page] : pages_map_) {
        if (page.size + actual_size <= kMaxPageSize) {
            new_pos = models::DocumentPosition{
                index,      // page_index
                page.size,  // page_offset
            };
            break;
        }
    }

    if (!new_pos.has_value()) {
        const auto new_index = ++page_index_counter_;
        new_pos = models::DocumentPosition{
            new_index,  // page_index
            0,          // page_offset
        };
    }

    // TODO 
    // append payload
    // set is_active=False for the old payload
    // cleanup old file if needed
    // return new position

    return {};
}

models::DocumentInfoMap FileStorageSink::LoadMeta() {
    LOG_INFO() << "Loading documents info from FS";

    auto stream = OpenMetaFileIn(meta_path_);

    std::string prefix{};
    try {
        stream >> prefix;
    } catch (const std::runtime_error& ex) {}
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
        result[document_info.id] = std::make_shared<models::DocumentInfo>(std::move(document_info));
    }
    return result;
}

models::DocumentPayloadPtr FileStorageSink::LoadPayload(const models::DocumentId& document_id) {
    // TODO take position
    // find document
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
