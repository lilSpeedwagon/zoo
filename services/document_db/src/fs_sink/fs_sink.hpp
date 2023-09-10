#pragma once

#include <filesystem>
#include <optional>
#include <unordered_map>

#include <boost/noncopyable.hpp>

#include <fs_sink/page.hpp>
#include <models/document.hpp>


namespace documents::fs_sink {

/// @brief Database file storage adapter. Stores document index and
/// payloads to specific file structures.
class FileStorageSink : private boost::noncopyable {
public:
    FileStorageSink(const std::filesystem::path& path);
    FileStorageSink(FileStorageSink&& other);
    ~FileStorageSink();

    FileStorageSink& operator=(FileStorageSink&& other);

    /// @brief Init file system manager.
    void Init();

    /// @brief Reset manager state.
    void Reset();

    /// @brief stores document info index to FS (should be process-safe)
    void Store(const models::DocumentInfoMap& document_infos);

    /// @brief stores document payload to FS
    /// @param current_position current DocumentPosition
    /// @param payload_ptr a pointer to DocumentPayload to store
    /// @return new DocumentPosition where the document is stored
    models::DocumentPosition Store(const std::optional<models::DocumentPosition>& old_position_opt,
                                   const models::DocumentPayloadPtr payload_ptr);

    /// @brief Deletes document payload.
    /// @param position document payload position
    void Delete(const models::DocumentPosition& position);

    /// @brief Loads meta data from FS
    models::DocumentInfoMap LoadMeta();

    /// @brief Loads document payload by document id
    models::DocumentPayloadPtr LoadPayload(const models::DocumentPosition& position);

private:

    /// @brief inits FS files in the current directory
    void InitFs();

    /// @brief Find position for a document of the specified size
    /// @param size document size
    /// @return available DocumentPosition
    models::DocumentPosition FindAvailablePosition(size_t size);


    /// @brief Lookup for a page with the specified index. If page is missing and `create_if_missing`
    /// is set to `true` then the page will be created. Otherwise `FileSystemException` exception will be thrown.
    /// @param index page index
    /// @param create_if_missing whether to create a page if missing
    /// @return found or created page
    PageFile FindPage(size_t index, bool create_if_missing = false);

    void Swap(FileStorageSink&& other);
    
    std::filesystem::path path_;
    std::filesystem::path meta_path_;
    size_t page_index_counter_;
    std::unordered_map<size_t, PageFile> pages_map_;
};

} // namespace documents::fs_sink
