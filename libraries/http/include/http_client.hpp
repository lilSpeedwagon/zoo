#pragma once

#include <string>

#include <boost/beast/core.hpp>

#include <http/include/models.hpp>

namespace http::client {

constexpr int kHttpVersion = 11;

class HttpClient {
public: 
    HttpClient(const std::string& host, int port = 80);
    HttpClient(HttpClient&& client);
    ~HttpClient();

    HttpClient& operator=(HttpClient&& client);

    Response Request(const Request& request);
private:
    HttpClient(const HttpClient&);
    HttpClient& operator=(const HttpClient&);
    void Swap(HttpClient&& other);

    std::string host_;
    std::string port_;
    std::shared_ptr<boost::asio::io_context> context_ptr_;
    boost::beast::flat_buffer buffer_;
};

} // namespace http::client