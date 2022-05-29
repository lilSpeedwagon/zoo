#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <common/include/json.hpp>

namespace api_config::models {

/// @struct Configuration of the single CRUD API
struct ApiConfig {
    std::string name{};
    std::optional<std::string> description{};
    common::json::json schema{};
};

// TODO put it somewhere else
inline void from_json(const common::json::json& data, models::ApiConfig& config) {
    auto schema = data.at("schema");
    schema.at("name").get_to(config.name);
    if (schema.contains("description")) {
        schema.at("description").get_to(config.description);
    }
    config.schema = schema.at("schema");
}

} // namespace api_config::models