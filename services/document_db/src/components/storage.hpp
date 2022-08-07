#pragma once

#include <atomic>
#include <shared_mutex>
#include <vector>
#include <unordered_map>

#include <boost/noncopyable.hpp>

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

    documents::models::DocumentPtr Get(models::DocumentId id, bool fetch_payload = false);
    std::vector<models::DocumentPtr> List();
    models::DocumentPtr Add(models::DocumentInput&& input);
    models::DocumentPtr Update(models::DocumentId id, models::DocumentUpdateInput&& input);
    documents::models::DocumentPtr Delete(models::DocumentId id);

private:
    models::DocumentPayloadPtr FetchPayload(models::DocumentId id);
    
    std::atomic<size_t> id_counter_;
    std::shared_mutex data_access_mutex_;
    std::unordered_map<models::DocumentId, models::DocumentPtr> documents_info_;
    std::unordered_map<models::DocumentId, models::DocumentPayloadPtr> payload_cache_;
};

} // namespace documents::components