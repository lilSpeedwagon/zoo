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

auto FindDocumentPayload(
    const std::unordered_map<models::DocumentId, models::DocumentPayloadPtr>& storage,
    models::DocumentId id) {
    if (const auto it = storage.find(id);
        it != storage.end()) {
        return it;
    }
    throw exceptions::InvalidStateException(
        common::format::Format("Payload for document {} not found", id));
}

} // namespace

Storage::Storage() : data_access_mutex_{}, id_counter_{0},
                     documents_info_{}, payload_cache_{}, sink_("./") {}

Storage::~Storage() {}

void Storage::Init() {
    documents_info_ = sink_.LoadMeta();
    // payload_cache_ = ...
}

void Storage::Reset() {
    boost::lock_guard lock(data_access_mutex_);
    documents_info_.clear();
    payload_cache_.clear();
}

const char* Storage::Name() const { return kName; };

documents::models::Document Storage::Get(models::DocumentId id, bool fetch_payload) {
    boost::shared_lock lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    models::Document result{};
    result.info = *info_it->second;
    if (fetch_payload) {
        const auto payload_it = FindDocumentPayload(payload_cache_, id);
        result.payload = *payload_it->second;
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
    };
    
    boost::lock_guard lock(data_access_mutex_);
    documents_info_[id] = std::make_shared<models::DocumentInfo>(info);
    payload_cache_[id] = 
        std::make_shared<models::DocumentPayload>(std::move(input.payload));
    OnDocumentUpdated();
    return models::Document{
        info,          // info
        std::nullopt,  // payload
    };
}

models::Document Storage::Update(models::DocumentId id,
                                 models::DocumentUpdateInput&& input) {
    boost::upgrade_lock read_lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    auto& info = *info_it->second;
    if (input.name.has_value() || input.namespace_name.has_value() ||
        input.payload.has_value()) {
        boost::upgrade_to_unique_lock write_lock(read_lock);
        if (input.name.has_value()) {
            info.name = std::move(input.name.value());
        }
        if (input.namespace_name.has_value()) {
            info.namespace_name = std::move(input.namespace_name.value());
        }
        if (input.payload.has_value()) {
            const auto payload_it = FindDocumentPayload(payload_cache_, id);
            *payload_it->second = std::move(input.payload.value());
        }
        info.updated = std::chrono::system_clock::now();
    }
    OnDocumentUpdated();
    return models::Document {
        info,           // info
        std::nullopt,   // payload
    };
}

models::Document Storage::Delete(models::DocumentId id) {
    boost::lock_guard lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    const auto payload_it = FindDocumentPayload(payload_cache_, id);
    models::Document result{
        std::move(*info_it->second), // info
        std::nullopt,               // payload
    };
    documents_info_.erase(info_it);
    payload_cache_.erase(payload_it);
    OnDocumentUpdated();
    return result;
}

models::DocumentId Storage::NextId() {
    return models::DocumentId{id_counter_.fetch_add(1, std::memory_order::memory_order_relaxed)};  
}

void Storage::OnDocumentUpdated() {
    sink_.Store(documents_info_);
}

} // namespace documents::components
