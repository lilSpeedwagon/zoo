#pragma once

#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <models.hpp>


namespace http::tcp {

/**
 * @class async TCP session wrapper intended for HTTP requests
 * handling.
 */ 
class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
    explicit TcpSession(const HttpHandler& on_request_ready,
                        boost::asio::ip::tcp::socket&& socket);
    void Run();
    
private:
    void AsyncRead();
    void OnRead(boost::beast::error_code error_code,
                std::size_t bytes_transferred);
    void OnWrite(const bool close, boost::beast::error_code error_code, 
                  std::size_t bytes_transferred);
    void Close();

    HttpHandler on_request_ready_;
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    Request request_;
    Response response_;
};

} // namespace http::tcp