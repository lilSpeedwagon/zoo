#include "list.hpp"

#include <components/include/components_engine.hpp>

#include <components/storage.hpp>
#include <models/document.hpp>
#include <utils/response.hpp>

namespace documents::handlers {

http::Response handle_list(http::Request&& request) {
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::Storage>();
    auto documents = storage_ptr->List();
    std::sort(
        documents.begin(), documents.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.info.id.GetUnderlying() < rhs.info.id.GetUnderlying();
        });
    return utils::response::ToResponse(std::move(documents));
}

} // namespace documents::handlers
