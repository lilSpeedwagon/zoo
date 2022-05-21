#include "http_server.hpp"

#include <boost/asio/strand.hpp>

#include <common/include/logging.hpp>
#include <common/include/format.hpp>

#include <exceptions.hpp>
#include <tcp_session/tcp_session.hpp>

namespace http::server {

namespace {

namespace boost_http = boost::beast::http;
namespace format = common::format;

using ErrorCode = boost::beast::error_code;

constexpr boost::string_view kServerVersion = "SelfMadeZoo Http 0.1";
constexpr boost::string_view kContentText = "text/html";

Response MakeBaseResponse(const unsigned version,
                          const boost_http::status status = boost_http::status::ok) {
    return Response{status, version};
}

Response MakeBaseResponse(Request&& request,
                          const boost_http::status status = boost_http::status::ok) {
    return Response{status, request.version()};
}

Response ServerErrorResponse(unsigned int version) {
    auto response = MakeBaseResponse(version, boost_http::status::internal_server_error);
    response.body() = "Server error.";
    return response;
};

Response NotFoundResponse(Request&& request) {
    auto response = MakeBaseResponse(std::move(request),
                                     boost_http::status::not_found);
    response.body() = "Not found.";
    return response;
};

Response ResponseFromHttpError(unsigned int version, 
                               const exceptions::HttpError& error) {
    auto response = MakeBaseResponse(version, error.Code());
    response.body() = error.what();
    return response;
}

void PrepareResponse(Response& response, bool keep_alive) {
    response.set(boost_http::field::server, kServerVersion);
    response.set(boost_http::field::content_type, kContentText);
    response.keep_alive(keep_alive);
    response.prepare_payload();
}

} // namespace

HttpHandlers::HttpHandlers() : handlers_{} {}

void HttpHandlers::AddHandler(const std::string& uri,
                              const Method method,
                              const HttpHandler& handler) {
    handlers_[uri][method] = handler;
}

void HttpHandlers::RemoveHandler(const std::string& uri, const Method method) {
    const auto uri_it = handlers_.find(uri);
    if (uri_it != handlers_.end()) {
        const auto items_erased = uri_it->second.erase(method);
        if (items_erased != 0 && uri_it->second.empty()) {
            handlers_.erase(uri);
        }
    }
}

std::optional<HttpHandler> HttpHandlers::GetHandler(
        const std::string& uri, const Method method) const {
    const auto uri_it = handlers_.find(uri);
    if (uri_it != handlers_.end()) {
        const auto handler_it = uri_it->second.find(method);
        if (handler_it != uri_it->second.end()) {
            return handler_it->second;
        }
    }
    return std::nullopt;
}

HttpServer::HttpServer(std::shared_ptr<boost::asio::io_context> io_context_ptr,
                       const std::string& address, const unsigned short port) :
    io_context_ptr_{io_context_ptr},
    acceptor_{*io_context_ptr_,
        boost::asio::ip::tcp::endpoint{boost::asio::ip::make_address(address), port}} {}

void HttpServer::AddListener(const std::string& uri, const Method verb,
                             const HttpHandler& handler) {
    LOG_DEBUG() << "setup handler " << uri;
    handlers_.AddHandler(uri, verb, handler);
}

void HttpServer::Listen() {
    ErrorCode error_code{};
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), error_code);
    if (error_code) {
        throw std::runtime_error(error_code.message());
    }
    
    acceptor_.listen(boost::asio::socket_base::max_listen_connections, error_code);
    if (error_code) {
        throw std::runtime_error(error_code.message());
    }

    LOG_INFO() << "HttpServer is listening for incoming connections on port " 
               << acceptor_.local_endpoint().port();

    AsyncAcceptNextConnection();
}

void HttpServer::AsyncAcceptNextConnection() {
    acceptor_.async_accept(
        boost::asio::make_strand(*io_context_ptr_),
        boost::beast::bind_front_handler(
            &HttpServer::OnConnectionAccepted,
            shared_from_this()));
}

void HttpServer::OnConnectionAccepted(
    const ErrorCode error_code, boost::asio::ip::tcp::socket socket) {

    if (!error_code) {
        auto on_request_ready = [this](Request&& request) {
            return this->HandleRequest(std::move(request));
        };
        std::make_shared<tcp::TcpSession>(on_request_ready, std::move(socket))->Run();
    } else {
        LOG_ERROR() << "error on accepting new connection: " << error_code.message();
    }

    // Accept another connection
    AsyncAcceptNextConnection();
}

Response HttpServer::HandleRequest(Request&& request) {
    LOG_DEBUG() << common::format::Format(">>> HTTP/{} {} {} {}",
        request.version(), request.method_string().to_string(),
        request.target().to_string(), request.body());

    const auto version = request.version();
    const auto keep_alive = request.keep_alive();
    Response response{};
    try {
        response = RouteRequest(std::move(request));
    } catch (const exceptions::HttpError& error) {
        response = ResponseFromHttpError(version, error);
    } catch (const std::exception& ex) {
        LOG_ERROR() << format::Format("not handled exception in {} {}: {}",
                                      request.method_string().to_string(),
                                      request.target().to_string(), ex);
        response = ServerErrorResponse(version);
    }
    PrepareResponse(response, keep_alive);

    LOG_DEBUG() << common::format::Format("<<< HTTP/{} {} {}",
        response.version(), response.result_int(), response.body());
    return response;
}

Response HttpServer::RouteRequest(Request&& request) {
    auto method = request.method();
    auto uri = std::string(request.target());
    auto handler = handlers_.GetHandler(uri, method);
    if (!handler.has_value()) {
        LOG_INFO() << format::Format("handler for {} {} not found",
                                     request.method_string().to_string(), uri);
        return NotFoundResponse(std::move(request));
    }
    return handler.value()(std::move(request));
}

} // namespace http::server