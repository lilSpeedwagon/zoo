#pragma once

#include <string>
#include <unordered_map>

#include <common/include/json.hpp>

#include <http/include/exceptions.hpp>
#include <http/include/models.hpp>

namespace http::utils {

std::string ToString(const Method method);

/// @brief Tries to parse request body as a json
/// and interpret as value of the specified type T.
/// @throws http::exceptions::BadRequest if json has
/// invalid structure or incompatible schema.
template<typename T>
T Parse(const Request& request) {
    try {
        auto json_data = common::json::json::parse(request.body());
        return json_data.get<T>();
    } catch (const common::json::detail::exception& ex) {
        throw exceptions::BadRequest(ex.what());
    }
}

/// @brief Get map of http request params.
std::unordered_map<std::string, std::string> GetParams(const Request& request);

} // namespace http::utils