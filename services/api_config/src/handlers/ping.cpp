#include "ping.hpp"

#include <common/include/logging.hpp>

namespace api_config::handlers {

http::Response handle_ping(http::Request&& request) {
    http::Response response{http::Status::ok, request.version()};
    LOG_INFO() << "/ping";
    response.body() = "OK";
    return response;
}

} // namespace api_config::handlers