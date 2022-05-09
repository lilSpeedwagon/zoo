#include "http_client.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <common/include/format.hpp>
#include <common/include/logging.hpp>
#include <http/include/utils.hpp>

namespace http::client {

namespace {

auto ResolveHost(boost::asio::io_context& context,
                 const std::string& host, const std::string& port) {
    boost::asio::ip::tcp::resolver resolver(context);
    return resolver.resolve(host, port);
}

} // namespace

HttpClient::HttpClient(const std::string& host, int port) 
    : host_{host}, port_{std::to_string(port)} {
        context_ptr_ = std::make_shared<boost::asio::io_context>();
    }

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
    if (context_ptr_ == nullptr) {
        throw std::runtime_error("IO context was not initialized");
    }

    LOG_DEBUG() << common::format::Format(
        "requesting {} {}:{}{}\n{}", http::utils::ToString(request.method()),
        host_, port_, request.target().to_string(), request.body());

    auto& context = *context_ptr_;
    auto const resolved_host = ResolveHost(context, host_, port_);
   
    boost::beast::tcp_stream stream(context);
    stream.connect(resolved_host);
    boost::beast::http::write(stream, request);

    Response result{};
    boost::beast::http::read(stream, buffer_, result);

    boost::beast::error_code ec{};
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if(ec && ec != boost::beast::errc::not_connected) {
        throw std::runtime_error{
            common::format::Format("Socket closure error: ", ec.message())};
    }

    LOG_DEBUG() << common::format::Format(
        "{} {}", result.result_int(), result.body());

    return result;
}

} // namespace http::client