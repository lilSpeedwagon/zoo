#pragma once

#include <http/include/models.hpp>

namespace http::handlers {

Response handle_ping(Request&& request);

} // namespace http::handlers