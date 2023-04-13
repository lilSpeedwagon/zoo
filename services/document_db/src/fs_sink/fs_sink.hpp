#pragma once

#include <filesystem>
#include <string>

#include <boost/noncopyable.hpp>

#include <common/include/logging.hpp>

#include <models/document.hpp>

namespace documents::fs_sink {

/// @brief Database file storage adapter. Stores document index and
/// payloads to specific file structures.
class FileStorageSink : private boost::noncopyable{
public:
    FileStorageSink(const std::string& path);
    ~FileStorageSink();

    /// @brief uploads data from FS to memory 
    void SyncWithFs();
    
    /// @brief stores document info index to FS (should be process-safe)
    void Store(const models::DocumentInfoMap& document_infos);

    /// @brief Loads meta data from FS
    models::DocumentInfoMap LoadMeta();

    // TODO store/load DocumentPayload

private:
    /// @brief inits FS files in the current directory
    void InitFs();
    
    std::filesystem::path path_;
    std::filesystem::path meta_path_;
};

} // namespace documents::fs_sink