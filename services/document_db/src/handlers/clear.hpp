#pragma once

#include <http/include/models.hpp>


namespace documents::handlers {

http::Response HandleClear(http::Request&& request);

} // namespace documents::handlers
