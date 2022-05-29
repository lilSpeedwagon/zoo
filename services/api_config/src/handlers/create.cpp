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

namespace {

// void from_json(const common::json::json& data, models::ApiConfig& config) {
//     auto schema = data.at("schema");
//     schema.at("name").get_to(config.name);
//     if (schema.contains("description")) {
//         schema.at("description").get_to(config.description);
//     }
//     config.schema = schema.at("schema");
// }

} // namespace

http::Response handle_create(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/create";
    auto config = http::utils::Parse<models::ApiConfig>(request);
    auto storage_ptr = common::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    try {
        storage_ptr->Insert(config);
    } catch (const std::runtime_error& ex) {
        LOG_INFO() << ex.what();
        throw http::exceptions::BadRequest(ex.what());
    }
    http::Response response{http::Status::ok, request.version()};
    response.body() = "OK";
    // TODO: put stored data to response
    return response;
}

} // namespace api_config::handlers