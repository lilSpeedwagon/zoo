#include "create.hpp"

#include <optional>

#include <common/include/components_engine.hpp>
#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/api_storage.hpp>
#include <models/api_config.hpp>

namespace api_config::handlers {

http::Response handle_create(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/create";
    auto config = http::utils::Parse<models::ApiConfig>(request);
    auto storage_ptr = common::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    
    std::optional<models::ApiConfigData> inserted{};
    try {
        inserted = storage_ptr->Insert(config);
    } catch (const std::runtime_error& ex) {
        LOG_INFO() << ex.what();
        throw http::exceptions::BadRequest(ex.what());
    }
    common::json::json response_json = inserted.value();
    http::Response response{http::Status::ok, request.version()};
    response.body() = response_json.dump();
    return response;
}

} // namespace api_config::handlers