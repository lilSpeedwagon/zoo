#include "delete.hpp"

#include <components/include/components_engine.hpp>

#include <components/storage.hpp>


namespace documents::handlers {

http::Response HandleClear(http::Request&& request) {
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::Storage>();
    const auto items_deleted = storage_ptr->Clear();

    http::Response response{};
    response.body() = common::json::json{{"items_deleted", items_deleted}}.dump();
    return response;
}

} // namespace documents::handlers
