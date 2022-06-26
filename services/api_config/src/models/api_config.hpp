#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <common/include/json.hpp>
#include <common/include/strong_typedef.hpp>

namespace api_config::models {

using ApiConfigId = common::types::StrongTypedef<uint64_t, struct IdTag>;

/// @struct Configuration of the single CRUD API
struct ApiConfig {
    std::string name{};
    std::optional<std::string> description{};
    common::json::json schema{};
};

struct Metadata {
    ApiConfigId id{};
    std::string author{};
    std::chrono::time_point<std::chrono::system_clock> created{};
    std::chrono::time_point<std::chrono::system_clock> updated{};
};

/// @struct Configuration of the single CRUD API stored in the service
struct ApiConfigData {
    ApiConfig data{};
    Metadata metadata{};
};

void from_json(const common::json::json& data, models::ApiConfig& config);
void to_json(common::json::json& json, const ApiConfigData& data);

} // namespace api_config::models