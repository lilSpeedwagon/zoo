#pragma once

#include <atomic>
#include <shared_mutex>
#include <vector>
#include <unordered_map>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include <components/include/component_base.hpp>

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

    documents::models::Document Get(models::DocumentId id, bool fetch_payload = false);
    std::vector<models::Document> List();
    models::Document Add(models::DocumentInput&& input);
    models::Document Update(models::DocumentId id, models::DocumentUpdateInput&& input);
    documents::models::Document Delete(models::DocumentId id);

private:
    models::DocumentId NextId();
    models::DocumentPayloadPtr FetchPayload(models::DocumentId id);
    
    // shared mutex implements multiple readers, single writer concept
    boost::upgrade_mutex data_access_mutex_;
    std::atomic<size_t> id_counter_;
    std::unordered_map<models::DocumentId, models::DocumentInfoPtr> documents_info_;
    std::unordered_map<models::DocumentId, models::DocumentPayloadPtr> payload_cache_;
};

} // namespace documents::components