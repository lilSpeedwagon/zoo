#include "page.hpp"

#include <string>

#include <common/include/format.hpp>
#include <common/include/logging.hpp>

#include <models/exceptions.hpp>


namespace documents::fs_sink {

namespace {

static constexpr std::string_view kPagePrefix = "PAGE";
static constexpr std::string_view kPageFilePrefix = "page_";
static constexpr std::string_view kPageFileExtension = ".dp";
static constexpr size_t kPagePrefixSize = sizeof(size_t) + kPagePrefix.size();  // move to binary traits
static constexpr size_t kPageInitialSize = kPagePrefixSize + sizeof(size_t);
static constexpr const std::ios_base::openmode kFileReadMode = std::ios::binary | std::ios::in;

size_t GetPageSize(const std::filesystem::path& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error& ex) {
        LOG_ERROR() << common::format::Format(
            "Cannot obtain page size for file {}: {}", path, ex.what());
        throw exceptions::FileSystemException();
    }
}

size_t GetPageIndex(const std::filesystem::path& path) {
    const auto stem = path.stem().string();
    try {
        return std::stol(stem.substr(kPageFilePrefix.size()));
    } catch (const std::logic_error& ex) {
        LOG_ERROR() << common::format::Format(
            "Cannot obtain page index for file {}: {}", path, ex.what());
        throw exceptions::FileSystemException();
    }
}

size_t GetActivePayloadsCount(const std::filesystem::path& path) {
    try {
        common::binary::BinaryInStream file(path);
        std::string buffer;
        file >> buffer;
        if (buffer != kPagePrefix) {
            LOG_ERROR() << path << " file is corrupted; found prefix \"" << buffer << "\"";
            throw exceptions::FileSystemException();
        }
        size_t count;
        file >> count;
        return count;
    } catch(const std::ios_base::failure& ex) {
        LOG_ERROR() << "I/O error on file " << path << ": " << ex.what();
        throw exceptions::FileSystemException();
    }
}

std::filesystem::path GetPagePath(const std::filesystem::path& root_path, size_t page_index) {
    return root_path / std::filesystem::path(common::format::Format(
        "{}{}{}", kPageFilePrefix, page_index, kPageFileExtension));
}

} // namespace

PageFile::PageFile(const std::filesystem::path& path) 
    : path_(path), index_(GetPageIndex(path)) {
    Init();
}

PageFile::PageFile(const std::filesystem::path& root_path, size_t index) 
    : path_(GetPagePath(root_path, index)), index_(index) {
    Init();
}

void PageFile::Init() {
    if (std::filesystem::exists(path_)) {
        size_ = GetPageSize(path_);
        payloads_count_ = GetActivePayloadsCount(path_);
    } else {
        size_ = GetDefaultPageSize();
        payloads_count_ = 0;

        LOG_DEBUG() << "Initializing new page at " << path_;
        common::binary::BinaryOutStream stream(path_);
        stream << kPagePrefix;
        stream << payloads_count_;
    }
}

PageFile::PageFile(const PageFile& other) 
    : path_(other.path_), size_(other.size_),
      payloads_count_(other.payloads_count_), index_(other.index_) {}

PageFile::PageFile(PageFile&& other) 
    : path_(std::move(other.path_)), size_(std::move(other.size_)),
      payloads_count_(std::move(other.payloads_count_)), index_(std::move(other.index_)) {}

PageFile::~PageFile() {}

PageFile& PageFile::operator=(const PageFile& other) {
    path_ = other.path_;
    size_ = other.size_;
    payloads_count_ = other.payloads_count_;
    index_ = other.index_;
    return *this;
}

PageFile& PageFile::operator=(PageFile&& other) {
    std::swap(path_, other.path_);
    std::swap(size_, other.size_);
    std::swap(payloads_count_, other.payloads_count_);
    std::swap(index_, other.index_);
    return *this;
}

void PageFile::StorePayload(models::DocumentPayloadPtr payload_ptr, size_t offset,
                            std::optional<size_t> old_offset_opt) {
    if (payload_ptr == nullptr) {
        LOG_ERROR() << "Missing payload to store";
        throw std::logic_error("");
    }

    const bool is_active = true;
    common::binary::BinaryOutStream file(path_);
    file.Seek(offset);
    file << is_active;
    file << *payload_ptr;
    size_ += sizeof(bool) + sizeof(size_t) + payload_ptr->GetUnderlying().size(); // TODO move to binary size traits
    payloads_count_++;

    // disable old payload if needed
    if (old_offset_opt.has_value()) {
        DisablePayload(file, old_offset_opt.value());
    }
}

void PageFile::DisablePayload(size_t offset) {
    common::binary::BinaryOutStream file(path_);
    DisablePayload(file, offset);
}

void PageFile::DisablePayload(common::binary::BinaryOutStream& file, size_t offset) {
    payloads_count_--;
    file.Seek(kPagePrefixSize);
    file << payloads_count_;

    const bool old_pos_is_active = false;
    file.Seek(offset);
    file << old_pos_is_active;

    assert(payloads_count_ >= 0);
    if (payloads_count_ == 0) {
        Delete();
    }
}

void PageFile::Delete() {
    std::filesystem::remove(path_);
    size_ = 0;
    payloads_count_ = 0;
}

models::DocumentPayloadPtr PageFile::LoadPayload(size_t page_offset) {
    common::binary::BinaryInStream file(path_);
    file.Seek(page_offset);
    bool is_active{};
    file >> is_active;
    if (!is_active) {
        LOG_ERROR() << "Document info points to an unactive payload: "
                    << path_ << ":" << page_offset;
        throw exceptions::FileSystemException();
    }
    models::DocumentPayload payload;
    file >> payload;
    return std::make_shared<models::DocumentPayload>(std::move(payload));
}

std::filesystem::path PageFile::Path() const {
    return path_;
}

size_t PageFile::Size() const {
    return size_;
}

size_t PageFile::Index() const {
    return index_;
}

size_t PageFile::GetDefaultPageSize() {
    return kPageInitialSize;
}

} // namespace documents::fs_sink
