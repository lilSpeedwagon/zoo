#pragma once

#include <http/include/models.hpp>

#include <models/api_config.hpp>

namespace api_config::utils {

models::ApiConfigId GetId(const http::Request& request);

} // namespace api_config::utils