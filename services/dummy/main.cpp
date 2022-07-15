#include <thread>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <common/include/config/logging_config.hpp>
#include <common/include/logging.hpp>
#include <common/include/format.hpp>
#include <http/include/consts.hpp>
#include <http/include/http_server.hpp>
#include <http/include/models.hpp>

#include <handlers/get.hpp>
#include <handlers/ping.hpp>

static constexpr const char* kAddress = "127.0.0.1";
static const unsigned short kPort = 1111;
static const size_t kThreadPoolSize = 1;

common::logging::LoggerController InitLogger() {
    auto log_config = common::config::GetLogConfig();
    common::logging::LoggerController controller(log_config);
    LOG_DEBUG() << "Logger is ready";
    return controller;
}

int main() {
    setlocale(LC_ALL, "Russian");

    constexpr size_t kThreadPoolSize = 1;
    
    try {
        auto log_controller = InitLogger();

        LOG_INFO() << "Setting up the server...";
        auto io_context_ptr = std::make_shared<boost::asio::io_context>(kThreadPoolSize);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            io_context_ptr, http::consts::kLocalhost, kPort);
        server_ptr->AddListener("/get", http::Method::get, &dummy::handlers::handle_get);
        server_ptr->AddListener("/ping", http::Method::get, &dummy::handlers::handle_ping);
        server_ptr->Listen();

        io_context_ptr->run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << common::format::ToString(ex);
        return 1;
    }

    return 0;
}