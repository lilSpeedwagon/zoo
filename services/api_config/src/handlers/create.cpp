#include "create.hpp"

#include <optional>

#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <http/include/utils.hpp>

#include <models/api_config.hpp>

namespace api_config::handlers {

namespace {

/// @struct Configuration of the single CRUD API
/// for /api-config/create request
struct ApiConfigInput {
    std::string name{};
    std::optional<std::string> description{};
    common::json::json schema{};
};

void from_json(const common::json::json& data, ApiConfigInput& config) {
    data.at("name").get_to(config.name);
    if (data.contains("description")) {
        data.at("description").get_to(config.description);
    }
    config.schema = data.at("schema");
}

} // namespace

http::Response handle_create(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/create";
    auto config = http::utils::Parse<ApiConfigInput>(request);
    // TODO store to some persistent map or something

    http::Response response{http::Status::ok, request.version()};
    response.body() = "OK";
    return response;
}

} // namespace api_config::handlers