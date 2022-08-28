#include "create.hpp"

#include <optional>

#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/exceptions.hpp>
#include <http/include/utils.hpp>

#include <components/storage.hpp>
#include <models/document.hpp>
#include <utils/response.hpp>

namespace documents::handlers {

namespace {

models::DocumentInput ParseRequest(http::Request&& request) {
    try {
        models::DocumentInput document{};
        auto data = common::json::json::parse(request.body());
        document.name = std::move(data.at("name").get_ref<std::string&>());
        document.owner = std::move(data.at("owner").get_ref<std::string&>());
        document.namespace_name = std::move(data.at("namespace").get_ref<std::string&>());
        document.payload = std::move(data.at("payload").get_ref<std::string&>());
        return document;
    } catch (const common::json::detail::exception& ex) {
        throw http::exceptions::BadRequest(ex.what());
    }
}

} // namespace

http::Response handle_create(http::Request&& request) {
    auto document = ParseRequest(std::move(request));
    auto storage_ptr = ::components::ComponentsEngine::GetInstance()
        .Get<components::Storage>();
    auto inserted = storage_ptr->Add(std::move(document));
    return utils::response::ToResponse(std::move(inserted));
}

} // namespace documents::handlers
