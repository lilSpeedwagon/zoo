#include "fs_sink.hpp"

#include <exception>
#include <filesystem>
#include <fstream>

#include <common/include/binary.hpp>
#include <common/include/logging.hpp>
#include <common/include/format.hpp>
#include <common/include/utils/errors.hpp>

#include <fs_sink/db_binary.hpp>


namespace documents::fs_sink {

// Documents index (DocumentInfo + DocumentPosition) is stored to a separate file meta.ddb.
// Document payloads are stored to a number of data files. Indexed DocumentPosition
// provides information about the exact location of a specific DocumentPayload. 

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

void FileStorageSink::InitFs() {
    LOG_INFO() << "Init document DB file system storage at "
               << meta_path_.generic_string();

    const auto is_index_exists = std::filesystem::exists(meta_path_);
    if (!is_index_exists) {
        // touch index file
        OpenMetaFileOut(meta_path_);
    }
}

} // namespace documents::fs_sink
