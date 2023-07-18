#pragma once

#include <filesystem>
#include <optional>

#include <common/include/binary.hpp>

#include <models/document.hpp>


namespace documents::fs_sink {

/// @brief Meta info about a single DB payload page file.
class PageFile {
public:
    /// @brief Ctor. Creates page file if it doesn't exist.
    /// @param path path to the page file 
    PageFile(const std::filesystem::path& path);

    /// @brief Ctor. Creates page file if it doesn't exist.
    /// @param root_path path to the pages root folder
    /// @param index page index
    PageFile(const std::filesystem::path& root_path, size_t index);

    PageFile(const PageFile& other);
    PageFile(PageFile&& other);

    ~PageFile();

    PageFile& operator=(const PageFile& other);
    PageFile& operator=(PageFile&& other);

    /// @brief Stores payload to the Page with the specified offset.
    /// @param payload_ptr smart pointer to the payload to store
    /// @param offset payload offset within the page file
    /// @param old_offset previously stored payload offset if exist
    void StorePayload(
        models::DocumentPayloadPtr payload_ptr, size_t offset, std::optional<size_t> old_offset = std::nullopt);

    /// @brief Disables previously stored payload
    /// @param offset payload offset within the page file
    void DisablePayload(size_t offset);

    /// @brief Load payload data from the page file.
    /// @param page_offset offset to the stored payload in the file
    /// @return smart pointer to payload data
    models::DocumentPayloadPtr LoadPayload(size_t page_offset);

    std::filesystem::path Path() const;
    size_t Size() const;
    size_t Index() const;

    /// @brief Get size of a just created page file.
    static size_t GetDefaultPageSize();

private:
    void Init();
    void DisablePayload(common::binary::BinaryOutStream& file, size_t offset);
    void Delete();

    std::filesystem::path path_;
    size_t size_;
    size_t payloads_count_;
    size_t index_;
};

} // namespace documents::fs_sink
