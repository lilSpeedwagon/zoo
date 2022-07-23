#pragma once

#include <http/include/models.hpp>

namespace dummy::handlers {

http::Response handle_dict_add(http::Request&& request);

http::Response handle_dict_get(http::Request&& request);

} // namespace namespace dummy::handlers