#include "create.hpp"

#include <optional>

#include <common/include/json.hpp>
#include <common/include/logging.hpp>

namespace api_config::handlers {

namespace {

/// @struct Configuration of the single CRUD API
/// for /api-config/create request
struct ApiConfigInput {
    std::string name{};
    std::optional<std::string> description{};
    common::json schema{};
};


} // namespace

http::Response handle_create(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/create";


    http::Response response{http::Status::ok, request.version()};
    
    response.body() = "OK";
    return response;
}

} // namespace api_config::handlers