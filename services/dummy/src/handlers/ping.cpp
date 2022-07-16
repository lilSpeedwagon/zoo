#include "ping.hpp"

#include <common/include/logging.hpp>

namespace dummy::handlers {

http::Response handle_ping(http::Request&& request) {
    http::Response response{http::Status::ok, request.version()};
    LOG_INFO() << "/ping";
    response.body() = "OK";
    return response;
}

} // namespace dummy::handlers