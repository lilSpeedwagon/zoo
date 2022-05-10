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

} // namespace http::utils