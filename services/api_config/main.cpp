#include <iostream>

#include <common/include/config/logging_config.hpp>
#include <common/include/format.hpp>
#include <common/include/logging.hpp>
#include <common/include/thread_pool.hpp>
#include <http/include/http_server.hpp>

#include <handlers/ping.hpp>

static constexpr const char* kAddress = "127.0.0.1";
static constexpr unsigned short kPort = 80;

common::logging::LoggerController InitLogger() {
    auto log_config = common::config::GetLogConfig();
    common::logging::LoggerController controller(log_config);
    LOG_DEBUG() << "Logger is ready";
    return controller;
}

int main() {
    const size_t kThreadsCount = 4;

    try {
        const auto log_controller = InitLogger();
        LOG_INFO() << "Setting up the server...";
        common::threading::IoThreadPool pool(kThreadsCount);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            pool.GetContextPtr(), kAddress, kPort);
        server_ptr->AddListener("/ping", http::Method::get,
                                &api_config::handlers::handle_ping);
        server_ptr->Listen();
        pool.RunInThisThread();
    } catch (const std::exception& ex) {
        std::cerr << common::format::ToString(ex);
        return 1;
    }

    return 0;
}