#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <common/include/json.hpp>

namespace api_config::models {

/// @struct Configuration of the single CRUD API
struct ApiConfig {
    uint64_t id{};
    std::string name{};
    std::optional<std::string> decription{};
    std::string author{};
    std::chrono::time_point<std::chrono::system_clock> created{};
    std::chrono::time_point<std::chrono::system_clock> updated{};
    common::json::json schema{};
};

} // namespace api_config::models