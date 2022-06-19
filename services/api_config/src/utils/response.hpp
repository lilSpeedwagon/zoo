#pragma once

#include <common/include/json.hpp>
#include <http/include/models.hpp>

#include <models/api_config.hpp>


namespace api_config::utils {

http::Response ToResponse(const models::ApiConfigData& config);

} // namespace api_config::utils