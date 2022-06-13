#include <string>
#include <unordered_map>

#include <catch2/catch.hpp>

#include <http/include/models.hpp>
#include <http/include/http_server.hpp>

namespace http::tests::http_server {

TEST_CASE("AddListener", "[HttpHandlers]") {
    http::HttpHandler get_handler = [](http::Request&&) {
        http::Response response(http::Status::ok, 10);
        response.body() = "get handler";
        return response; 
    };
    http::HttpHandler post_handler = [](http::Request&&) {
        http::Response response(http::Status::ok, 10);
        response.body() = "post handler";
        return response; 
    };

    http::server::HttpHandlers handlers{};
    handlers.AddHandler("/path/get", http::Method::get, get_handler);
    handlers.AddHandler("/path/post", http::Method::post, post_handler);
    
    auto get_handler_opt = handlers.GetHandler("/path/get", http::Method::get);
    CHECK(get_handler_opt.has_value());
    CHECK(get_handler_opt->operator()(Request{}).body() == std::string("get handler"));

    auto post_handler_opt = handlers.GetHandler("/path/post", http::Method::post);
    CHECK(post_handler_opt.has_value());
    CHECK(post_handler_opt->operator()(Request{}).body() == std::string("post handler"));

    auto invalid_handler_opt = handlers.GetHandler("/path/post", http::Method::get);
    CHECK(!invalid_handler_opt.has_value());

    auto invalid_path_opt = handlers.GetHandler("/path/invalid", http::Method::get);
    CHECK(!invalid_path_opt.has_value());
}


} // namespace http::tests::http_server