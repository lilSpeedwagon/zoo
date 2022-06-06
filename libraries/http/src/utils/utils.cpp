#include "http/include/utils.hpp"

#include <common/include/format.hpp>

namespace http::utils {

std::string ToString(const Method method) {
    switch(method) {
        case Method::get:
            return "GET";
        case Method::post:
            return "POST";
        case Method::put:
            return "PUT";
        case Method::delete_:
            return "DELETE";
        case Method::head:
            return "HEAD";
        default:
            return common::format::Format(
                "HTTP Method {}", static_cast<int>(method));
    }
}

std::unordered_map<std::string, std::string> GetParams(const Request& request) {
    static const char kPathArgumentsPrefix = '?';
    static const char kPathArgumentsDelimiter = '&';
    static const char kPathArgumentsEqual = '=';

    std::unordered_map<std::string, std::string> params{};
    const auto url = std::string(request.target());
    auto it = std::find(url.cbegin(), url.cend(), kPathArgumentsPrefix);

    while (it != url.cend()) {
        auto equal_it = std::find(std::next(it), url.cend(), kPathArgumentsEqual);
        if (equal_it == url.cend()) {
            break;
        }
        auto delimiter_it = std::find(std::next(equal_it), url.cend(), kPathArgumentsDelimiter);
        auto key = std::string(std::next(it), equal_it);
        auto value = std::string(std::next(equal_it), delimiter_it);
        if (!key.empty()) {
            params[std::move(key)] = std::move(value);
        }
        it = delimiter_it;
    }
    return params;
}

} // namespace http::utils