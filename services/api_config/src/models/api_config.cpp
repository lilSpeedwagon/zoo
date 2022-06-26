#include "api_config.hpp"

#include <common/include/format.hpp>

namespace api_config::models {

void from_json(const common::json::json& data, models::ApiConfig& config) {
    auto schema = data.at("schema");
    schema.at("name").get_to(config.name);
    if (schema.contains("description")) {
        schema.at("description").get_to(config.description);
    }
    config.schema = schema.at("schema");
}

void to_json(common::json::json& json, const ApiConfigData& data) {
    json = common::json::json{
        {"id", data.metadata.id.GetUnderlying()},                       // id
        {"author", data.metadata.author},                               // author
        {"created", common::format::ToString(data.metadata.created)},   // created
        {"updated", common::format::ToString(data.metadata.updated)},   // updated
        {"name", data.data.name},                                       // name
        {"schema", data.data.schema},                                   // schema
    };
    if (data.data.description.has_value()) {
        json["description"] = data.data.description.value();
    }
}

} // namespace api_config::models