#include "storage.hpp"

#include <common/include/format.hpp>

#include <models/exceptions.hpp>

namespace documents::components {

namespace {

auto FindDocumentInfo(
    const std::unordered_map<models::DocumentId, models::DocumentPtr>& storage,
    models::DocumentId id) {
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

// Current implementation is an in-memory prototype.
// TODO: move the storage into a filesystem.

Storage::Storage() {

}

Storage::~Storage() {}

void Storage::Init() {}

void Storage::Reset() {
    std::lock_guard lock(data_access_mutex_);
    documents_info_.clear();
    payload_cache_.clear();
}

const char* Storage::Name() const { return kName; };

documents::models::DocumentPtr Storage::Get(models::DocumentId id, bool fetch_payload) {
    std::shared_lock lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    auto result_ptr = std::make_shared<models::Document>(*info_it->second);
    if (fetch_payload) {
        const auto payload_it = FindDocumentPayload(payload_cache_, id);
        result_ptr->payload_ptr = payload_it->second;
    }
    return result_ptr;
}

std::vector<models::DocumentPtr> Storage::List() {
    std::vector<models::DocumentPtr> result{};
    result.reserve(documents_info_.size());
    std::shared_lock lock(data_access_mutex_);
    for (const auto& [_, document_ptr] : documents_info_) {
        result.push_back(document_ptr);
    }
    return result;
}

models::DocumentPtr Storage::Add(models::DocumentInput&& input) {
    const auto id = models::DocumentId{
        id_counter_.fetch_add(1, std::memory_order::memory_order_relaxed)};    
    auto created = std::chrono::system_clock::now();
    auto updated = std::chrono::system_clock::now();

    models::Document document{
        id,                               // id
        std::move(created),               // created
        std::move(updated),               // updated
        std::move(input.name),            // name
        std::move(input.owner),           // owner
        std::move(input.namespace_name),  // namespace_name
        nullptr,                          // payload_ptr
    };
    auto document_ptr = std::make_shared<models::Document>(std::move(document));
    
    std::lock_guard lock(data_access_mutex_);
    documents_info_[id] = document_ptr;
    payload_cache_[id] = std::make_shared<models::DocumentPayload>(input.payload);
    return document_ptr;
}

models::DocumentPtr Storage::Update(models::DocumentId id,
                                    models::DocumentUpdateInput&& input) {
    std::lock_guard lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    auto& document = *info_it->second;
    document.name = std::move(input.name);
    document.namespace_name = std::move(input.namespace_name);
    document.updated = std::chrono::system_clock::now();

    models::DocumentPtr result_ptr = std::make_shared<models::Document>(document);
    if (input.payload.has_value()) {
        const auto payload_it = FindDocumentPayload(payload_cache_, id);
        *payload_it->second = input.payload.value();
        result_ptr->payload_ptr = payload_it->second;
    }
    return result_ptr;
}

documents::models::DocumentPtr Storage::Delete(models::DocumentId id) {
    std::lock_guard lock(data_access_mutex_);
    const auto info_it = FindDocumentInfo(documents_info_, id);
    const auto payload_it = FindDocumentPayload(payload_cache_, id);
    auto result_ptr = info_it->second;
    result_ptr->payload_ptr = payload_it->second;
    documents_info_.erase(info_it);
    payload_cache_.erase(payload_it);
    return result_ptr;
}

} // namespace documents::components