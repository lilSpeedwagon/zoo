#pragma once

#include <http/include/models.hpp>

namespace dummy::handlers {

http::Response handle_get(http::Request&& request);

http::Response handle_get_parametrized(http::Request&& request);

} // namespace namespace dummy::handlers