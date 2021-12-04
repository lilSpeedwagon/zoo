#include <thread>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

static constexpr const char* kAddress = "127.0.0.1";
static constexpr unsigned short kPort = 80;

#define LOG() (std::cout << "t_id[" << std::this_thread::get_id() << "] ")

using HttpHandler = std::function<http::response<http::string_body>(
    http::request<http::string_body>&&)>;

class HttpHandlers {
public:
    using MethodToHandler = std::unordered_map<http::verb, HttpHandler>;
    using UriToMethod = std::unordered_map<std::string, MethodToHandler>;

    HttpHandlers() : handlers_{} {}

    void AddHandler(const std::string& uri, const http::verb verb, const HttpHandler& handler) {
        handlers_[uri][verb] = handler;
    }

    std::optional<HttpHandler> GetHandler(const std::string& uri, const http::verb verb) const {
        const auto uri_it = handlers_.find(uri);
        if (uri_it != handlers_.end()) {
            const auto handler_it = uri_it->second.find(verb);
            if (handler_it != uri_it->second.end()) {
                return handler_it->second;
            }
        }
        return std::nullopt;
    }

private:
    UriToMethod handlers_{};
};


http::response<http::string_body> handle_ping(
    http::request<http::string_body>&& req) {
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/plain");
    res.body() = "OK";
    res.keep_alive(req.keep_alive());
    return res;
}

http::response<http::string_body> handle_http_request(
    http::request<http::string_body>&& req) {
    // Returns a bad request response
    auto const bad_request =
    [&req](beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found =
    [&req](beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error =
    [&req](beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if( req.method() != http::verb::get) {
        return bad_request("Unknown HTTP-method");
    }

    if (req.target() != "/ping") {
        return bad_request("Illegal request-target");
    }

    // Respond to GET request
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/plain");
    res.body() = "OK";
    res.keep_alive(req.keep_alive());
    return res;
}



// Handles an HTTP server connection
class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
    using OnRequestReadyT = std::function<http::response<http::string_body>(
        http::request<http::string_body>&&)>;

    // Take ownership of the stream
    TcpSession(const OnRequestReadyT& on_request_ready,
        tcp::socket&& socket)
        : stream_(std::move(socket)), buffer_(),
          req_(), res_(), on_request_ready_(on_request_ready) {}

    // Start the asynchronous operation
    void run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(stream_.get_executor(),
                      beast::bind_front_handler(
                          &TcpSession::do_read,
                          shared_from_this()));
    }

    void do_read() {
        LOG() << "do read\n";

        // Make the request empty before reading,
        // otherwise the operation behavior is undefined.
        req_.clear();

        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));

        // Read a request
        http::async_read(stream_, buffer_, req_,
            beast::bind_front_handler(
                &TcpSession::on_read,
                shared_from_this()));
    }

    void on_read(beast::error_code error_code,
                 std::size_t bytes_transferred) {
        LOG() << "on read\n";

        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if(error_code == http::error::end_of_stream)
            return do_close();
        if(error_code) {
            LOG() << "cannot read data: " << error_code.message() << "\n";
            return;
        }

        auto response = on_request_ready_(std::move(req_));
        const bool close = response.need_eof();
        LOG() << "response " << response.body() << "\n";

        res_ = std::move(response);

        //response_ptr_ = std::make_shared<http::serializer<false, http::string_body>>(response);

        // Write the response
        http::async_write(
            stream_, res_,
            beast::bind_front_handler(
                &TcpSession::on_write,
                shared_from_this(),
                response.need_eof()));
    }

    void on_write(const bool close, beast::error_code error_code, 
                  std::size_t bytes_transferred) {
        LOG() << "on write\n";

        boost::ignore_unused(bytes_transferred);

        if(error_code) {
            LOG() << "cannot write response: " << error_code.message() << '\n';
        }

        if(close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // We're done with the response so delete it
        //response_ptr_.reset();

        // Read another request
        do_read();
    }

    void
    do_close()
    {
        LOG() << "do close\n";
        // Send a TCP shutdown
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

private:
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
    OnRequestReadyT on_request_ready_;
    //std::shared_ptr<http::serializer<false, http::string_body>> response_ptr_;
};

void handle_tcp_connection(tcp::socket& socket) {
    LOG() << "new tcp connection\n";

    // This buffer is required to persist across reads
    beast::flat_buffer buffer{};
    beast::error_code error_code{};

    for(;;)
    {
        LOG() << "reading the data\n";

        // Read a request
        http::request<http::string_body> req;
        http::read(socket, buffer, req, error_code);
        if (error_code == http::error::end_of_stream) {
            LOG() << "no data\n";
            break;
        }
        if (error_code) {
            LOG() << "cannot read data: " << error_code.message() << "\n";
            break;
        }
            
        LOG() << "new requst: " << req.target() << "\nbody: " << req.body() << "\n";

        // Handle request
        auto response = handle_http_request(std::move(req));
        const bool close = response.need_eof();
        LOG() << "response " << response.body() << "\n";

        // Send the response
        http::serializer<false, http::string_body> sr{response};
        beast::error_code error_code{};
        http::write(socket, sr, error_code);

        if (error_code) {
            LOG() << "cannot write response: " << error_code << "\n";
        }
        if (close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            LOG() << "close\n";
            break;
        }
    }

    // Send a TCP shutdown
    socket.shutdown(tcp::socket::shutdown_send, error_code);
    LOG() << "tcp connection closed\n";
}


class HttpServer : public std::enable_shared_from_this<HttpServer> {
public:

    explicit HttpServer(std::shared_ptr<net::io_context> io_context_ptr,
                        const std::string& address, const unsigned short port) :
        io_context_ptr_{io_context_ptr},
        acceptor_{*io_context_ptr_, tcp::endpoint{net::ip::make_address(kAddress), port}} {}

    void listen() {
        beast::error_code error_code{};
        acceptor_.set_option(net::socket_base::reuse_address(true), error_code);
        if (error_code) {
            LOG() << "cannot set up http server: " << error_code.message() << '\n';
            return;
        }
        
        acceptor_.listen(net::socket_base::max_listen_connections, error_code);
        if (error_code) {
            LOG() << "cannot start listening: " << error_code.message() << '\n';
            return;
        }

        do_accept();

        /*for (;;) {
            tcp::socket socket{io_context_};
            acceptor_.accept(socket);
            std::thread t(std::bind(handle_tcp_connection, std::move(socket)));
            t.detach();
            //handle_tcp_connection(std::move(socket));
        }*/
        LOG() << "server is listening\n";
    }

    void AddListener(const std::string& uri, const http::verb verb, const HttpHandler& handler) {
        handlers_.AddHandler(uri, verb, handler);
    }

private:
    void do_accept() {
        LOG() << "do async accept\n";
        acceptor_.async_accept(
            net::make_strand(*io_context_ptr_),
            beast::bind_front_handler(
                &HttpServer::on_accept,
                shared_from_this()));
    }

    void on_accept(const beast::error_code ec, tcp::socket socket) {
        LOG() << "on accept\n";
        if (ec) {
            LOG() << "cannot accept connection: " << ec.message() << '\n';
        } else {
            // Create the session and run it
            auto on_request_ready = [this](http::request<http::string_body>&& request) {
                return this->HandleRequest(std::move(request));
            };
            //std::bind(&HttpServer::HandleRequest, this);
            std::make_shared<TcpSession>(on_request_ready, std::move(socket))->run();
        }

        // Accept another connection
        do_accept();
    }

    http::response<http::string_body> HandleRequest(http::request<http::string_body>&& request) {
        try {
            return RouteRequest(std::move(request));
        } catch (const std::exception& ex) {
            LOG() << ex.what();
            return ServerErrorResponse(request);
        }
    }

    http::response<http::string_body> RouteRequest(http::request<http::string_body>&& request) {
        auto method = request.method();
        auto uri = std::string(request.target());
        auto handler = handlers_.GetHandler(uri, method);
        if (!handler.has_value()) {
            return NotFoundResponse(std::move(request));
        }

        return handler.value()(std::move(request));
    }

    static http::response<http::string_body> NotFoundResponse(const http::request<http::string_body>& request) {
        http::response<http::string_body> res{http::status::not_found, request.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = "Not found.";
        res.prepare_payload();
        return res;
    };

    static http::response<http::string_body> ServerErrorResponse(const http::request<http::string_body>& request) {
        http::response<http::string_body> res{http::status::internal_server_error, request.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = "Server error.";
        res.prepare_payload();
        return res;
    };

    std::shared_ptr<net::io_context> io_context_ptr_;
    tcp::acceptor acceptor_;
    HttpHandlers handlers_;
};

int main() {
    LOG() << "setting up the server...\n";
    setlocale(LC_ALL, "Russian");

    const size_t kThreadPoolSize = 10;
    
    try {
        auto io_context_ptr = std::make_shared<net::io_context>(kThreadPoolSize);
        auto server_ptr = std::make_shared<HttpServer>(io_context_ptr, kAddress, kPort);
        server_ptr->AddListener("/ping", http::verb::get, &handle_ping);
        server_ptr->listen();

        std::vector<std::thread> thread_pool;
        thread_pool.reserve(kThreadPoolSize - 1);
        for(auto i = kThreadPoolSize - 1; i > 0; --i)
            thread_pool.emplace_back(
            [io_context_ptr] {
                io_context_ptr->run();
            });
        io_context_ptr->run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}