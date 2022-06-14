#include "get.hpp"

#include <common/include/components_engine.hpp>
#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <common/include/utils/algo.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/api_storage.hpp>
#include <models/api_config.hpp>

namespace api_config::handlers {

namespace {
    
uint64_t GetId(const http::Request& request) {
    auto params = http::utils::GetParams(request);
    auto id_opt = common::utils::algo::GetOptional(params, "id");
    if (!id_opt.has_value()) {
        throw http::exceptions::BadRequest("Parameter 'id' not found");
    }

    // negative numbers
    if (id_opt->at(0) == '-') {
        throw http::exceptions::BadRequest("Parameter 'id' is invalid");
    }
    
    try {
        return std::stoul(id_opt.value().data());
    } catch (const std::logic_error& ex) {
        throw http::exceptions::BadRequest("Parameter 'id' is invalid");
    }
}

} // namespace

http::Response handle_get(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/get";
    
    const auto id = GetId(request);
    LOG_INFO() << common::format::Format("get config with id {}", id);

    auto storage_ptr = common::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    auto config_opt = storage_ptr->Get(id);
    
    if (!config_opt.has_value()) {
        throw http::exceptions::NotFound(
            common::format::Format("API config with id '{}' not found", id).data());
    }
    common::json::json response_json = config_opt.value();
    http::Response response{http::Status::ok, request.version()};
    response.body() = response_json.dump();
    return response;
}

} // namespace api_config::handlers