#include <thread>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <http/include/http_server.hpp>
#include <http/include/models.hpp>

static constexpr const char* kAddress = "127.0.0.1";
static constexpr unsigned short kPort = 80;

#define LOG() (std::cout << "t_id[" << std::this_thread::get_id() << "] ")

http::Response handle_ping(http::Request&& request) {
    http::Response response{http::Status::ok, request.version()};
    LOG() << "/ping";
    response.body() = "OK";
    return response;
}

int main() {
    LOG() << "setting up the server...\n";
    setlocale(LC_ALL, "Russian");

    constexpr size_t kThreadPoolSize = 10;
    
    try {
        auto io_context_ptr = std::make_shared<boost::asio::io_context>(kThreadPoolSize);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            io_context_ptr, kAddress, kPort);
        server_ptr->AddListener("/ping", http::Method::get, &handle_ping);
        server_ptr->Listen();

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