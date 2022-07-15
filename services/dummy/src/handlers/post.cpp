#include "post.hpp"

#include <optional>

#include <common/include/logging.hpp>
#include <common/include/json.hpp>
#include <http/include/utils.hpp>

namespace dummy::handlers {

namespace {

struct RequestData {
    int number{};
    std::string string{};
    bool flag{};
    std::optional<int> nullable_value{};
};

void from_json(const common::json::json& json, RequestData& data) {
    json.at("number").get_to(data.number);
    json.at("string").get_to(data.string);
    json.at("flag").get_to(data.flag);
    if (json.contains("nullable_value")) {
        data.nullable_value = json.at("nullable_value").get<int>();
    }
}

} // namespace

http::Response handle_post(http::Request&& request) {
    LOG_INFO() << "/test/post";
    RequestData data{};
    try {
        auto json = common::json::json::parse(request.body());
        data = json.get<RequestData>();
    } catch (const common::json::detail::exception&) {
        throw http::exceptions::BadRequest("Bad request");
    }

    common::json::json response_json = {{"result", "success"}};
    http::Response response{};
    response.body() = response_json.dump();
    return response;
}

} // namespace dummy::handlers