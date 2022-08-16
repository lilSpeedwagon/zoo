#pragma once

#include <http/include/models.hpp>

namespace documents::handlers {

http::Response handle_update(http::Request&& request);

} // namespace documents::handlers