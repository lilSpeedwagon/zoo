#include "http_client.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace http::client {

namespace {

auto ResolveHost(boost::asio::io_context& context,
                 const std::string& host, const std::string& port) {
    boost::asio::ip::tcp::resolver resolver(context);
    return resolver.resolve(host, port);
}

} // namespace

HttpClient::HttpClient(const std::string& host, int port) 
    : host_{host}, port_{std::to_string(port)}, context_ptr_{} {}

HttpClient::HttpClient(HttpClient&& client) {
    Swap(std::move(client));
}

HttpClient::~HttpClient() {}

HttpClient& HttpClient::operator=(HttpClient&& client) {
    Swap(std::move(client));
    return *this;
}

void HttpClient::Swap(HttpClient&& other) {
    std::swap(host_, other.host_);
    std::swap(port_, other.port_);
    std::swap(buffer_, other.buffer_);
    other.context_ptr_.swap(context_ptr_);
}

Response HttpClient::Request(const http::Request& request) {
    auto& context = *context_ptr_;
    auto const resolved_host = ResolveHost(context, host_, port_);
   
    boost::beast::tcp_stream stream(context);
    stream.connect(resolved_host);
    boost::beast::http::write(stream, request);

    Response result{};
    boost::beast::http::read(stream, buffer_, result);

    boost::beast::error_code ec{};
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if(ec /*&& ec != boost::beast::errc::not_connected*/) { // this error may be ok
        throw std::runtime_error{ec.message()};
    }

    return result;
}

} // namespace http::client