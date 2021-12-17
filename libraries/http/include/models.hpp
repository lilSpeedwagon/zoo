#pragma once

#include <boost/beast/http.hpp>


namespace http {

using StringBody = boost::beast::http::string_body;
using Request = boost::beast::http::request<StringBody>;
using Response = boost::beast::http::response<StringBody>;
using Method = boost::beast::http::verb;
using Status = boost::beast::http::status;
using HttpHandler = std::function<Response(Request&&)>;

} // namespace http