#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include <common/include/logging.hpp>

#include <models/document.hpp>


namespace documents::fs_sink {

/// @brief Database file storage adapter. Stores document index and
/// payloads to specific file structures.
class FileStorageSink : private boost::noncopyable {
public:
    struct PageFile {
        std::filesystem::path path;
        size_t size;
    };

    FileStorageSink(const std::filesystem::path& path);
    FileStorageSink(FileStorageSink&& other);
    ~FileStorageSink();

    FileStorageSink& operator=(FileStorageSink&& other);

    /// @brief stores document info index to FS (should be process-safe)
    void Store(const models::DocumentInfoMap& document_infos);

    /// @brief stores document payload to FS
    /// @param current_position current DocumentPosition
    /// @param payload_ptr a pointer to DocumentPayload to store
    /// @return new DocumentPosition where the document is stored
    models::DocumentPosition Store(const models::DocumentPosition& current_position,
                                   const models::DocumentPayloadPtr payload_ptr);

    /// @brief Loads meta data from FS
    models::DocumentInfoMap LoadMeta();

    /// @brief Loads document payload by document id
    models::DocumentPayloadPtr LoadPayload(const models::DocumentId& document_id);

private:

    /// @brief inits FS files in the current directory
    void InitFs();

    models::DocumentPosition FindNewPosition(const std::filesystem::path& path, size_t payload_size);

    void Swap(FileStorageSink&& other);
    
    std::filesystem::path path_;
    std::filesystem::path meta_path_;
    size_t page_index_counter_;
    std::unordered_map<size_t, PageFile> pages_map_;
};

} // namespace documents::fs_sink
