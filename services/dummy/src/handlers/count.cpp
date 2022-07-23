#include "count.hpp"

#include <common/include/logging.hpp>
#include <common/include/json.hpp>
#include <components/include/components_engine.hpp>

#include <components/counter.hpp>

namespace dummy::handlers {

http::Response handle_count(http::Request&& request) {
    LOG_INFO() << "/test/count";
    
    auto& engine = ::components::ComponentsEngine::GetInstance();
    auto counter_ptr = engine.Get<components::DummyCounter>();
    const auto result = counter_ptr->AddOne();
    
    common::json::json response_json = {{"result", result}};
    http::Response response{};
    response.body() = response_json.dump();
    return response;
}

} // namespace dummy::handlers