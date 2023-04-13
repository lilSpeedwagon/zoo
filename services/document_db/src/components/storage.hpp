#pragma once

#include <atomic>
#include <shared_mutex>
#include <vector>
#include <unordered_map>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include <components/include/component_base.hpp>

#include <fs_sink/fs_sink.hpp>
#include <models/document.hpp>


namespace documents::components {

/// @class Documents storage manager controlling document CRUD operations.
class Storage : public ::components::ComponentBase, private boost::noncopyable {
public:
    static constexpr const char* kName = "storage";

    Storage();
    virtual ~Storage();

    void Init() override;
    void Reset() override;
    const char* Name() const override;

    /// @brief Retrieves a document by its id.
    /// @param id document id
    /// @param fetch_payload fetch document payload or not
    /// @return document
    /// @throws NotFoundException if document is not found
    models::Document Get(models::DocumentId id, bool fetch_payload = false);

    /// @brief Retrieves all existing documents.
    /// @return vector of documents
    std::vector<models::Document> List();

    /// @brief Stores a document.
    /// @param input document data
    /// @return stored document
    models::Document Add(models::DocumentInput&& input);
    
    /// @brief Updates previously stored document.
    /// @param id document id
    /// @param input document data to update
    /// @return updated document
    /// @throws NotFoundException if document is not found
    models::Document Update(models::DocumentId id, models::DocumentUpdateInput&& input);

    /// @brief Deletes previously stored document
    /// @param id 
    /// @return deleted document
    /// @throws NotFoundException if document is not found
    documents::models::Document Delete(models::DocumentId id);
    
    /// @brief Deletes all of the stored documents.
    /// @return number of deleted documents
    size_t Clear();

private:
    models::DocumentId NextId();
    models::DocumentPayloadPtr FetchPayload(models::DocumentId id);
    void OnDocumentUpdated();
    void Load();
    void Unload();
    
    // shared mutex implements multiple readers, single writer concept
    boost::upgrade_mutex data_access_mutex_;
    std::atomic<size_t> id_counter_;
    models::DocumentInfoMap documents_info_;

    // for now it is in-memory data storage
    // TODO store payloads in FS, apply some LRU cache
    std::unordered_map<models::DocumentId, models::DocumentPayloadPtr> payload_cache_;
    fs_sink::FileStorageSink sink_;
};

} // namespace documents::components
