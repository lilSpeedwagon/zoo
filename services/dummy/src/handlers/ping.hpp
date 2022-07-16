#pragma once

#include <http/include/models.hpp>

namespace dummy::handlers {

http::Response handle_ping(http::Request&& request);

} // namespace dummy::handlers