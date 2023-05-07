#include "storage.hpp"

#include <common/include/format.hpp>

#include <models/exceptions.hpp>


namespace documents::components {

namespace {

auto FindDocumentInfo(
    const models::DocumentInfoMap& storage, models::DocumentId id) {
    if (const auto it = storage.find(id);
        it != storage.end()) {
        return it;
    }
    throw exceptions::NotFoundException(id);
}

} // namespace

Storage::Storage() : data_access_mutex_{}, id_counter_{0},
                     documents_info_{}, sink_("./") {}  // TODO move this path to configs

Storage::~Storage() {
    Unload();
}

void Storage::Init() {
    Load();
}

void Storage::Reset() {
    Unload();
    Load();
}

const char* Storage::Name() const { return kName; };

documents::models::Document Storage::Get(models::DocumentId id, bool fetch_payload) {
    boost::shared_lock lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    models::Document result{};
    result.info = *info_it->second;
    if (fetch_payload) {
        // TODO: need to get rid of this if, position must be always presented
        if (!result.info.position.has_value()) {
            throw std::runtime_error("Missing document position");
        }

        const auto payload_ptr = sink_.LoadPayload(result.info.position.value());
        result.payload = *payload_ptr;
    }
    return result;
}

std::vector<models::Document> Storage::List() {
    std::vector<models::Document> result{};
    result.reserve(documents_info_.size());
    boost::shared_lock lock(data_access_mutex_);
    for (const auto& [_, info_ptr] : documents_info_) {
        result.push_back(
            models::Document{
                *info_ptr,      // info
                std::nullopt,   // payload
            });
    }
    return result;
}

models::Document Storage::Add(models::DocumentInput&& input) {  
    const auto id = NextId();
    auto created = std::chrono::system_clock::now();
    auto updated = created;

    models::DocumentInfo info{
        id,                               // id
        std::move(created),               // created
        std::move(updated),               // updated
        std::move(input.name),            // name
        std::move(input.owner),           // owner
        std::move(input.namespace_name),  // namespace_name
        std::nullopt,                     // position
    };

    auto payload_ptr = std::make_shared<models::DocumentPayload>(std::move(input.payload));
    auto info_ptr = std::make_shared<models::DocumentInfo>(info);
    
    boost::lock_guard lock(data_access_mutex_);
    documents_info_[id] = info_ptr;

    OnDocumentUpdated(info_ptr, payload_ptr);
    return models::Document{
        std::move(info),  // info
        std::nullopt,     // payload
    };
}

models::Document Storage::Update(models::DocumentId id,
                                 models::DocumentUpdateInput&& input) {
    boost::upgrade_lock read_lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    auto info_ptr = info_it->second;
    if (input.name.has_value() || input.namespace_name.has_value() ||
        input.payload.has_value()) {
        boost::upgrade_to_unique_lock write_lock(read_lock);
        if (input.name.has_value()) {
            info_ptr->name = std::move(input.name.value());
        }
        if (input.namespace_name.has_value()) {
            info_ptr->namespace_name = std::move(input.namespace_name.value());
        }
        info_ptr->updated = std::chrono::system_clock::now();
    }

    if (input.payload.has_value()) {
        auto payload_ptr = std::make_shared<models::DocumentPayload>(std::move(input.payload.value()));
        OnDocumentUpdated(info_ptr, payload_ptr);
    } else {
        OnDocumentUpdated();
    }

    return models::Document {
        *info_ptr,      // info
        std::nullopt,   // payload
    };
}

models::Document Storage::Delete(models::DocumentId id) {
    boost::lock_guard lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    models::Document result{
        std::move(*info_it->second), // info
        std::nullopt,                // payload
    };
    documents_info_.erase(info_it);
    OnDocumentUpdated();
    return result;
}

size_t Storage::Clear() {
    boost::lock_guard lock(data_access_mutex_);
    auto count = documents_info_.size();
    Unload();
    OnDocumentUpdated();
    return count;
}

models::DocumentId Storage::NextId() {
    return models::DocumentId{id_counter_.fetch_add(1, std::memory_order::memory_order_relaxed)};  
}

void Storage::OnDocumentUpdated() {
    sink_.Store(documents_info_);
}

void Storage::OnDocumentUpdated(const models::DocumentInfoPtr info_ptr, const models::DocumentPayloadPtr& payload_ptr) {
    auto position = sink_.Store(info_ptr->position, payload_ptr);
    info_ptr->position = std::move(position);
    OnDocumentUpdated();
}

void Storage::OnDocumentDeleted(const models::DocumentInfoPtr info_ptr) {
    if (info_ptr->position.has_value()) {
        sink_.Delete(info_ptr->position.value());
    } else {
        // TODO position always must present
        throw std::logic_error("missing document position on delete");
    }
}

void Storage::Load() {
    sink_.Init();
    documents_info_ = sink_.LoadMeta();
    RestoreIdCounter();
}

void Storage::Unload() {
    documents_info_.clear();
    sink_.Reset();
}

void Storage::RestoreIdCounter() {
    if (documents_info_.empty()) {
        id_counter_ = 0;
        return;
    }

    size_t max_id = 0;
    for (const auto& [id, _] : documents_info_) {
        if (auto id_value = id.GetUnderlying(); id_value > max_id) {
            max_id = id_value;
        }
    }
    id_counter_ = max_id + 1;
}

} // namespace documents::components
