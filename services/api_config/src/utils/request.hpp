#pragma once

#include <http/include/models.hpp>


namespace api_config::utils {

uint64_t GetId(const http::Request& request);

} // namespace api_config::utils