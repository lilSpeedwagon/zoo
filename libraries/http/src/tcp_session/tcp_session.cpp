#include "tcp_session.hpp"

#include <boost/asio/dispatch.hpp>

namespace http::tcp {

namespace {

constexpr std::chrono::seconds kDefaultOperationTimeout(10);

} // namespace


TcpSession::TcpSession(const HttpHandler& on_request_ready,
                       boost::asio::ip::tcp::socket&& socket)
    : on_request_ready_{on_request_ready}, stream_{std::move(socket)},
      buffer_{}, request_{}, response_{} {}

void TcpSession::Run() {
    // perform async I/O operations within a strand
    boost::asio::dispatch(stream_.get_executor(),
                          boost::beast::bind_front_handler(
                              &TcpSession::AsyncRead,
                              shared_from_this()));
}

void TcpSession::AsyncRead() {
    request_.clear();
    stream_.expires_after(kDefaultOperationTimeout);
    boost::beast::http::async_read(
        stream_, buffer_, request_,
        boost::beast::bind_front_handler(
            &TcpSession::OnRead,
            shared_from_this()));
}

void TcpSession::OnRead(boost::beast::error_code error_code,
                        std::size_t /*bytes_transferred*/) {
    if (error_code == boost::beast::http::error::end_of_stream) {
        Close();
        return;
    }

    if (error_code) {
        // throw or move exception to the main thread???
        return;
    }

    response_ = on_request_ready_(std::move(request_));
    const bool close = response_.need_eof();
    boost::beast::http::async_write(
        stream_, response_,
        boost::beast::bind_front_handler(
            &TcpSession::OnWrite,
            shared_from_this(), close));
}

void TcpSession::OnWrite(const bool close, boost::beast::error_code error_code, 
                         std::size_t /*bytes_transferred*/) {
    if (error_code) {
        // LOG or something
    }

    if (close) {
        Close();
        return;
    }

    AsyncRead();
}

void TcpSession::Close() {
    boost::beast::error_code error;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, error);
    if (error) {
        // Log or throw
    }
}

} // namespace http::tcp
