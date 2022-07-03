#include "update.hpp"

#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/api_storage.hpp>
#include <models/api_config.hpp>
#include <utils/response.hpp>

namespace api_config::handlers {

http::Response handle_update(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/update";

    models::ApiConfigId id{};
    models::ApiConfig config{};
    try {
        auto data = common::json::json::parse(request.body());
        id = models::ApiConfigId(data.at("id"));
        config = data.get<models::ApiConfig>();
    } catch (const common::json::detail::exception& ex) {
        throw http::exceptions::BadRequest(ex.what());
    }

    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    auto updated_opt = storage_ptr->Update(id, config);
    if (!updated_opt.has_value()) {
        throw http::exceptions::NotFound(
            common::format::Format("API config with id '{}' not found", id).data());
    }

    return utils::ToResponse(updated_opt.value());
}

} // namespace api_config::handlers