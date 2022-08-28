#include "get.hpp"

#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>

#include <components/storage.hpp>
#include <models/document.hpp>
#include <models/exceptions.hpp>
#include <utils/request.hpp>
#include <utils/response.hpp>

namespace documents::handlers {

http::Response handle_get(http::Request&& request) {
    const auto id = utils::request::GetId(request);
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::Storage>();
    try {
        return utils::response::ToResponse(storage_ptr->Get(id, true));
    } catch (const exceptions::NotFoundException& ex) {
        throw http::exceptions::NotFound(ex.what());
    }
}

} // namespace documents::handlers
