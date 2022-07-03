#include "create.hpp"

#include <optional>

#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/api_storage.hpp>
#include <models/api_config.hpp>
#include <utils/response.hpp>

namespace api_config::handlers {

http::Response handle_create(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/create";
    auto config = http::utils::Parse<models::ApiConfig>(request);
    
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    auto inserted = storage_ptr->Insert(config);
    return utils::ToResponse(inserted);
}

} // namespace api_config::handlers