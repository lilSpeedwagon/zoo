#include <http/include/default_handlers.hpp>

namespace http::handlers {

Response handle_ping(Request&& request) {
    http::Response response{};
    response.body() = "OK";
    return response;
}

} // namespace http::handlers