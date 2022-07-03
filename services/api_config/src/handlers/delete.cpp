#include "delete.hpp"

#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>

#include <components/api_storage.hpp>
#include <models/api_config.hpp>
#include <utils/request.hpp>
#include <utils/response.hpp>

namespace api_config::handlers {

http::Response handle_delete(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/delete";
    
    const auto id = utils::GetId(request);
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    auto deleted_opt = storage_ptr->Delete(id);
    if (!deleted_opt.has_value()) {
        throw http::exceptions::NotFound(
            common::format::Format("API config with id '{}' not found", id).data());
    }
    
    return utils::ToResponse(deleted_opt.value());
}

} // namespace api_config::handlers