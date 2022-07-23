#include "dict.hpp"

#include <string>

#include <common/include/logging.hpp>
#include <common/include/json.hpp>
#include <common/include/utils/algo.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/dict.hpp>

namespace dummy::handlers {

namespace {
    
struct AddRequest {
    std::string key{};
    std::string value{};
};

void from_json(const common::json::json& data, AddRequest& request) {
    data.at("key").get_to(request.key);
    data.at("value").get_to(request.value);
}

} // namespace

http::Response handle_dict_add(http::Request&& request) {
    LOG_INFO() << "/test/dict-add";
    const auto add_request = http::utils::Parse<AddRequest>(request);
    
    auto& engine = ::components::ComponentsEngine::GetInstance();
    auto dict_ptr = engine.Get<components::DummyDict>();
    dict_ptr->Add(add_request.key, add_request.value);

    return http::Response{};
}

http::Response handle_dict_get(http::Request&& request) {
    LOG_INFO() << "/test/dict-get";
    const auto params = http::utils::GetParams(request);
    auto key_opt = common::utils::algo::GetOptional(params, "key");
    if (!key_opt.has_value()) {
        throw http::exceptions::BadRequest("Missing required param \'key\'");
    }
    
    auto& engine = ::components::ComponentsEngine::GetInstance();
    auto dict_ptr = engine.Get<components::DummyDict>();
    auto value_opt = dict_ptr->Get(key_opt.value());
    
    common::json::json json_data = {{"value", nullptr}};
    if (value_opt.has_value()) {
        json_data["value"] = value_opt.value();
    }
    http::Response response{};
    response.body() = json_data.dump();
    return response;
}

} // namespace dummy::handlers