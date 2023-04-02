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

/// @class Documents storage manager controls documents CRUD operations.
class Storage : public ::components::ComponentBase, private boost::noncopyable {
public:
    static constexpr const char* kName = "storage";

    Storage();
    virtual ~Storage();

    void Init() override;
    void Reset() override;
    const char* Name() const override;

    models::Document Get(models::DocumentId id, bool fetch_payload = false);
    std::vector<models::Document> List();
    models::Document Add(models::DocumentInput&& input);
    models::Document Update(models::DocumentId id, models::DocumentUpdateInput&& input);
    documents::models::Document Delete(models::DocumentId id);

private:
    models::DocumentId NextId();
    models::DocumentPayloadPtr FetchPayload(models::DocumentId id);
    void OnDocumentUpdated();
    
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
