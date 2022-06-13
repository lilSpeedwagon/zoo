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

http::Response handle_list(http::Request&& request) {
    LOG_INFO() << "/api/v1/api-config/list";

    auto storage_ptr = common::components::ComponentsEngine::GetInstance()
        .Get<components::ApiConfigStorage>();
    auto api_list = storage_ptr->List();
    
    common::json::json items{};
    for (auto& api : api_list) {
        items.push_back(api);
    }
    common::json::json result{{"items", std::move(items)}};

    http::Response response{http::Status::ok, request.version()};
    response.body() = result.dump();
    return response;
}

} // namespace api_config::handlers