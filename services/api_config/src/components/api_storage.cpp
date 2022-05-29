#include "api_storage.hpp"

#include <common/include/format.hpp>

namespace api_config::components {

ApiConfigStorage::ApiConfigStorage() : id_counter_{0} {}
ApiConfigStorage::~ApiConfigStorage() {}

const char* ApiConfigStorage::Name() const {
    return kName;
}

void ApiConfigStorage::Init() {}

void ApiConfigStorage::Insert(const models::ApiConfig& api) {
    const auto it = apis_.find(api.name);
    if (it != apis_.end()) {
        throw std::runtime_error(common::format::Format(
            "item with name '{}' already exists", api.name));
    }

    const auto now = std::chrono::system_clock::now();
    Metadata meta {
        GetNextId(), // id
        "author",    // author
        now,         // created
        now,         // updated
    };
    ApiConfigData data {
        api,             // data
        std::move(meta), // metadata
    };
    apis_[api.name] = std::move(data);
}

bool ApiConfigStorage::Update(const models::ApiConfig& api) {
    const auto it = apis_.find(api.name);
    if (it != apis_.end()) {
        it->second.metadata.updated = std::chrono::system_clock::now();
        it->second.data = api;
        return true;
    }
    return false;
}

bool ApiConfigStorage::Delete(const std::string& name) {
    return apis_.erase(name) != 0;
}

std::optional<ApiConfigData> ApiConfigStorage::Get(
    const std::string& name) const {
    if (auto it = apis_.find(name);
        it != apis_.end()) {
        return it->second;
    }
    return std::nullopt;
}

uint64_t ApiConfigStorage::GetNextId() {
    return id_counter_++;
}

} // namespace api_config::components