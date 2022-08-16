#include "response.hpp"

#include <common/include/format.hpp>
#include <common/include/json.hpp>

namespace documents::utils::response {

namespace {

common::json::json ToJson(models::Document&& document) {
    auto data = common::json::json{
        {"id", document.info.id.GetUnderlying()},
        {"owner", std::move(document.info.owner)},
        {"created", common::format::ToString(document.info.created)},
        {"updated", common::format::ToString(document.info.updated)},
        {"name", std::move(document.info.name)},
        {"namespace", std::move(document.info.namespace_name)},
        {"payload", nullptr},   
    };
    if (document.payload.has_value()) {
        data["payload"] = std::move(document.payload.value().GetUnderlying());
    }
    return data;
}

} // namespace

http::Response ToResponse(models::Document&& document) {
    http::Response response{};
    response.body() = ToJson(std::move(document)).dump();
    return response;
}

http::Response ToResponse(std::vector<models::Document>&& documents) {
    auto data = common::json::json::array();
    for (auto& item : documents) {
        data.push_back(ToJson(std::move(item)));
    }
    http::Response response{};
    response.body() = data.dump();
    return response;
}

} // namespace documents::utils::response