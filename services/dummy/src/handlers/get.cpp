#include "get.hpp"

#include <common/include/logging.hpp>
#include <common/include/json.hpp>
#include <http/include/utils.hpp>

namespace dummy::handlers {

http::Response handle_get(http::Request&& request) {
    LOG_INFO() << "/test/get";
    common::json::json response_json = {{"test_data", "hello world"}};
    http::Response response{};
    response.body() = response_json.dump();
    return response;
}

http::Response handle_get_parametrized(http::Request&& request) {
    LOG_INFO() << "/test/get_parametrized";
    auto params = http::utils::GetParams(request);
    common::json::json response_json = {{"params", params}};
    http::Response response{};
    response.body() = response_json.dump();
    return response;
}

} // namespace dummy::handlers