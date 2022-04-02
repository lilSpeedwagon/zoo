#include <thread>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <common/include/logging.hpp>
#include <common/include/format.hpp>

#include <http/include/http_server.hpp>
#include <http/include/models.hpp>

static constexpr const char* kAddress = "127.0.0.1";
static constexpr unsigned short kPort = 80;

http::Response handle_ping(http::Request&& request) {
    http::Response response{http::Status::ok, request.version()};
    LOG_INFO() << "/ping";
    response.body() = "OK";
    return response;
}

http::Response handle_echo(http::Request&& request) {
    http::Response response{http::Status::ok, request.version()};
    LOG_INFO() << "/echo";
    response.body() = request.body();
    return response;
}

common::logging::LoggerController InitLogger() {
    common::logging::LogSettings settings{};
    settings.buffer_max_size = 100;
    settings.flush_delay = std::chrono::milliseconds(100);
    settings.log_level = common::logging::LogLevel::Debug;
    settings.log_to_stdout = true;
    settings.path = "./";
    settings.file_prefix = "log_";
    common::logging::LoggerController controller(settings);
    LOG_DEBUG() << "Logger is ready";
    return controller;
}

int main() {
    setlocale(LC_ALL, "Russian");

    constexpr size_t kThreadPoolSize = 10;
    
    try {
        auto log_controller = InitLogger();

        LOG_INFO() << "Setting up the server...";
        auto io_context_ptr = std::make_shared<boost::asio::io_context>(kThreadPoolSize);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            io_context_ptr, kAddress, kPort);
        server_ptr->AddListener("/ping", http::Method::get, &handle_ping);
        server_ptr->AddListener("/echo", http::Method::post, &handle_echo);
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

    return 0;
}