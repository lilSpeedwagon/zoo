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
    apis_[data.metadata.id] = data;
    return data;
}

std::optional<ApiConfigData> ApiConfigStorage::Update(
    uint64_t id, const models::ApiConfig& api) {
    const auto it = apis_.find(id);
    if (it != apis_.end()) {
        it->second.metadata.updated = std::chrono::system_clock::now();
        it->second.data = api;
        return it->second;
    }
    return std::nullopt;
}

std::optional<ApiConfigData> ApiConfigStorage::Delete(uint64_t id) {
    const auto it = apis_.find(id);
    if (it != apis_.end()) {
        auto data = it->second;
        apis_.erase(it);
        return data;
    }
    return std::nullopt;
}

std::optional<ApiConfigData> ApiConfigStorage::Get(uint64_t id) const {
    if (auto it = apis_.find(id);
        it != apis_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<models::ApiConfigData> ApiConfigStorage::List() const {
    std::vector<models::ApiConfigData> result{};
    result.reserve(apis_.size());
    for (const auto& [_, api] : apis_) {
        result.push_back(api);
    }
    return result;
}

uint64_t ApiConfigStorage::GetNextId() {
    return id_counter_++;
}

} // namespace api_config::components