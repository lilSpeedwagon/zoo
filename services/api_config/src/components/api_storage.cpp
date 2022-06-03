#include "api_storage.hpp"

#include <common/include/format.hpp>

namespace api_config::components {

namespace {

using models::ApiConfigData;
using models::Metadata;

} // namespace

ApiConfigStorage::ApiConfigStorage() : id_counter_{0} {}
ApiConfigStorage::~ApiConfigStorage() {}

const char* ApiConfigStorage::Name() const {
    return kName;
}

void ApiConfigStorage::Init() {}

ApiConfigData ApiConfigStorage::Insert(const models::ApiConfig& api) {
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
    apis_[api.name] = data;
    return data;
}

ApiConfigData ApiConfigStorage::Update(const models::ApiConfig& api) {
    const auto it = apis_.find(api.name);
    if (it != apis_.end()) {
        it->second.metadata.updated = std::chrono::system_clock::now();
        it->second.data = api;
        return it->second;
    }
    throw std::runtime_error(
        common::format::Format("Config '{}' not found", api.name));
}

ApiConfigData ApiConfigStorage::Delete(const std::string& name) {
    const auto it = apis_.find(name);
    if (it != apis_.end()) {
        auto data = it->second;
        apis_.erase(it);
        return data;
    }
    throw std::runtime_error(
        common::format::Format("Config '{}' not found", name));
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