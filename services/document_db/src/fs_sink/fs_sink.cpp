#include "fs_sink.hpp"

#include <exception>
#include <filesystem>
#include <fstream>

#include <common/include/logging.hpp>
#include <common/include/format.hpp>
#include <common/include/utils/errors.hpp>

namespace documents::fs_sink {

// index:
// page + offset
// update index structure on create/move document
// store index structure on disk
// store whole index every time?

namespace {

static const std::ios_base::openmode kFileWriteMode = std::ios::binary | std::ios::out;
static const std::ios_base::openmode kFileReadMode = std::ios::binary | std::ios::in;
static const std::string kMetaPrefix = "META";
static const char kMetaItemPrefix = 'I';

struct DocumentPosition {
    size_t page_index{};
    size_t page_offset{};
};

std::filesystem::path GetIndexPath(std::filesystem::path path) {
    path += "/meta.ddb";
    return path;
}

template<typename T>
std::ofstream& operator<<(std::ofstream& stream, const T&& value) {
    if constexpr (std::is_integral_v<T>) {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));     
    } else {
        stream << value;
    }
    return stream;
}

std::ofstream& operator<<(std::ofstream& stream,
                          const std::chrono::time_point<std::chrono::system_clock>& time_point) {
    auto count = time_point.time_since_epoch().count();
    stream.write(reinterpret_cast<const char*>(&count), sizeof(count)); 
    return stream;
}

std::ofstream& operator<<(std::ofstream& stream, const std::string& str) {
    stream << str.size() << str;
    return stream;
}

std::ofstream& operator<<(std::ofstream& stream, const models::DocumentInfo& info) {
    stream << kMetaItemPrefix;
    stream << info.id.GetUnderlying();
    stream << info.created;
    stream << info.updated;
    stream << info.name.size() << info.name;
    stream << info.namespace_name.size() << info.namespace_name.size();
    stream << info.owner.size() << info.owner;
    return stream;
}

template<typename T>
std::ifstream& operator>>(std::ifstream& stream, T&& value) {
    if constexpr (std::is_integral_v<T>) {
        stream.read(reinterpret_cast<const char*>(&value), sizeof(value));     
    } else {
        stream >> value;
    }
    return stream;
}

std::ifstream& operator>>(std::ifstream& stream,
                          std::chrono::time_point<std::chrono::system_clock>& time_point) {
    int64_t count{};
    stream >> count;
    time_point = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::seconds(count));
    return stream;
}

std::ifstream& operator>>(std::ifstream& stream, models::DocumentInfo& info) {
    char c{};
    stream >> c;
    if (c != kMetaItemPrefix) {
        throw std::runtime_error(
            common::format::Format("Invalid meta data item prefix: {}", c));
    }
    uint64_t id{};
    stream >> id;
    info.id = id;
    stream >> info.created;
    stream >> info.updated;
    stream >> info.name;
    stream >> info.namespace_name;
    stream >> info.owner;
    return stream;
}

std::ifstream& operator>>(std::ifstream& stream, std::string& str) {
    size_t count{};
    stream >> count;
    str.reserve(count);
    stream.read(str.data(), count);
    return stream;
}

} // namespace

FileStorageSink::FileStorageSink(const std::string& path) 
    : path_(path), meta_path_(GetIndexPath(path_)) {
    InitFs();
}

FileStorageSink::~FileStorageSink() {}

void FileStorageSink::SyncWithFs() {}
    
void FileStorageSink::Store(const models::DocumentInfoMap& documents_info_) {
    LOG_INFO() << "Storing updated documents info to FS";

    // TODO acquire FS mutex
    // TODO start FS transaction

    auto stream = OpenMetaFile(kFileWriteMode);
    stream << kMetaPrefix;
    stream << documents_info_.size();
    for (const auto& [_, info] : documents_info_) {
        stream << info;
    }

    // TODO commit/rollback FS transaction
    // TODO release FS mutex

    LOG_INFO() << "Storing completed";
}

models::DocumentInfoMap FileStorageSink::LoadMeta() {
    LOG_INFO() << "Loading documents info from FS";

    auto stream = OpenMetaFile(kFileReadMode);

    /*while(!stream.eof()) {
        std::string s{};
        stream >> s;
        LOG_DEBUG() << s;
    }
    return {};*/

    std::string prefix{};
    stream >> prefix;
    if (prefix.empty() && stream.eof()) {
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
        models::DocumentInfoPtr info_ptr{};
        stream >> *info_ptr;
        result[info_ptr->id] = info_ptr;
    }
    return result;
}

void FileStorageSink::InitFs() {
    LOG_INFO() << "Init document DB file system storage at "
               << meta_path_.generic_string();

    const auto is_index_exists = std::filesystem::exists(meta_path_);
    if (!is_index_exists) {
        // touch index file
        OpenMetaFile(std::ios::in | std::ios::binary);
    }
}

std::fstream FileStorageSink::OpenMetaFile(std::ios_base::openmode mode) {
    std::fstream stream(meta_path_, mode);
    if (!stream.is_open() || !stream.good()) {
        throw std::runtime_error(common::format::Format(
            "Cannot open document_db index file \"{}\": {}",
            meta_path_.generic_string(), common::utils::errors::GetLastError()));
    }
    return stream;
}


} // namespace documents::fs_sink
