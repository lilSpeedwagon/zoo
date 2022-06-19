#include "response.hpp"

#include <http/include/consts.hpp>

namespace api_config::utils {

http::Response ToResponse(const models::ApiConfigData& config) {
    common::json::json response_json = config;
    http::Response response{};
    response.body() = response_json.dump();
    return response;
}

} // namespace api_config::utils
