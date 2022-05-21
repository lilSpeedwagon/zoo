#pragma once

#include <http/include/models.hpp>

namespace api_config::handlers {

http::Response handle_ping(http::Request&& request);

} // namespace api_config::handlers