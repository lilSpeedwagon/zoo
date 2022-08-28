#include "update.hpp"

#include <common/include/json.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/storage.hpp>
#include <models/exceptions.hpp>
#include <utils/response.hpp>
#include <utils/request.hpp>

namespace documents::handlers {

namespace {

std::tuple<models::DocumentId, models::DocumentUpdateInput> ParseRequest(
    http::Request&& request) {
    try {
        auto data = common::json::json::parse(request.body());
        auto id_it = data.find("id");
        if (id_it == data.end()) {
            throw http::exceptions::BadRequest("Key \'id\' is required.");
        }
        const auto id = models::DocumentId{id_it->get<uint64_t>()};
        
        models::DocumentUpdateInput document{};
        if (auto it = data.find("name");
            it != data.end()) {
            document.name = std::move(it->get_ref<std::string&>());
        }
        if (auto it = data.find("namespace");
            it != data.end()) {
            document.namespace_name = std::move(it->get_ref<std::string&>());
        }
        if (auto it = data.find("payload");
            it != data.end()) {
            document.payload = std::move(it->get_ref<std::string&>());
        }
        return {id, std::move(document)}; 
    } catch (const common::json::detail::exception& ex) {
        throw http::exceptions::BadRequest(ex.what());
    }
}

} // namespace

http::Response handle_update(http::Request&& request) {
    auto [id, data_to_update] = ParseRequest(std::move(request));
    auto storage_ptr = ::components::ComponentsEngine::GetInstance().
        Get<components::Storage>();
    try {
        auto updated = storage_ptr->Update(id, std::move(data_to_update));
        return utils::response::ToResponse(std::move(updated));
    } catch (const exceptions::NotFoundException& ex) {
        throw http::exceptions::NotFound(ex.what());
    }
}

} // namespace documents::handlers