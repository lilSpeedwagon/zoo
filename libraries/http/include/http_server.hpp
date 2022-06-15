#pragma once

#include <string>
#include <optional>

#include <boost/asio/ip/tcp.hpp>

#include "models.hpp"


namespace http::server {

/**
 * @class container for HTTP handlers
 * HTTP handler = method + uri
 */
class HttpHandlers {
public:
    HttpHandlers();
    HttpHandlers(const HttpHandlers& other);
    HttpHandlers(HttpHandlers&& other);
    ~HttpHandlers();

    HttpHandlers& operator=(const HttpHandlers& other);
    HttpHandlers& operator=(HttpHandlers&& other);

    void AddHandler(const std::string& uri, const Method method, const HttpHandler& handler);
    void RemoveHandler(const std::string& uri, const Method method);
    std::optional<HttpHandler> GetHandler(const std::string& uri, const Method method) const;

private:
    using MethodToHandler = std::unordered_map<Method, HttpHandler>;
    using UriToMethod = std::unordered_map<std::string, MethodToHandler>;

    UriToMethod handlers_;
};

/**
 * @class async HTTP server
 */
class HttpServer : public std::enable_shared_from_this<HttpServer> {
public:
    HttpServer(std::shared_ptr<boost::asio::io_context> io_context_ptr,
               const std::string& address, const unsigned short port);
    HttpServer(HttpServer&& other);
    ~HttpServer();

    HttpServer& operator=(HttpServer&& other);

    /// @brief Start listening for incoming connections
    /// @throws std::runtime error if unable to setup server
    void Listen();

    /// @brief Registers a new handler for the specified uri
    void AddListener(const std::string& uri, const Method method,
                     const HttpHandler& handler);

private:
    HttpServer(const HttpServer& other);
    HttpServer& operator=(const HttpServer& other);

    void AsyncAcceptNextConnection();
    void OnConnectionAccepted(const boost::beast::error_code error_code,
                              boost::asio::ip::tcp::socket socket);

    Response HandleRequest(Request&& request);
    Response RouteRequest(Request&& request);

    std::shared_ptr<boost::asio::io_context> io_context_ptr_;
    boost::asio::ip::tcp::acceptor acceptor_;
    HttpHandlers handlers_;
};

} // namespace http::server