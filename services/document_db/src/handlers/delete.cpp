#include "delete.hpp"

#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>

#include <components/storage.hpp>
#include <models/document.hpp>
#include <models/exceptions.hpp>
#include <utils/response.hpp>
#include <utils/request.hpp>

namespace documents::handlers {

namespace {

models::DocumentId ParseRequest(http::Request&& request) {
    try {
        auto data = common::json::json::parse(request.body());
        return models::DocumentId{data.at("id").get<uint64_t>()};
    } catch (const common::json::detail::exception& ex) {
        throw http::exceptions::BadRequest(ex.what());
    }
}

} // namespace

http::Response handle_delete(http::Request&& request) {
    const auto id = ParseRequest(std::move(request));
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::Storage>();
    try {
        auto deleted = storage_ptr->Delete(id);
        return utils::response::ToResponse(std::move(deleted));
    } catch (const exceptions::NotFoundException& ex) {
        throw http::exceptions::NotFound(ex.what());
    }
}

} // namespace documents::handlers