#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <common/include/config/logging_config.hpp>
#include <common/include/logging.hpp>
#include <components/include/components_controller.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/consts.hpp>
#include <http/include/http_server.hpp>
#include <http/include/models.hpp>

#include <components/counter.hpp>
#include <components/dict.hpp>
#include <handlers/count.hpp>
#include <handlers/dict.hpp>
#include <handlers/get.hpp>
#include <handlers/ping.hpp>
#include <handlers/post.hpp>

static const unsigned short kPort = 1111;
static const size_t kThreadPoolSize = 1;

common::logging::LoggerController InitLogger() {
    auto log_config = common::config::GetLogConfig();
    common::logging::LoggerController controller(log_config);
    LOG_DEBUG() << "Logger is ready";
    return controller;
}

auto InitComponents() {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto counter_ptr = std::make_shared<dummy::components::DummyCounter>();
    auto dict_ptr = std::make_shared<dummy::components::DummyDict>();
    engine.Register(counter_ptr);
    engine.Register(dict_ptr);
    engine.Init();

    auto controller_ptr = std::make_shared<components::ComponentsController>();
    controller_ptr->RunAsync();
    return controller_ptr;
}

int main() {
    setlocale(LC_ALL, "Russian");

    constexpr size_t kThreadPoolSize = 1;
    
    try {
        auto log_controller = InitLogger();
        auto components_controller = InitComponents();

        LOG_INFO() << "Setting up the server...";
        auto io_context_ptr = std::make_shared<boost::asio::io_context>(kThreadPoolSize);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            io_context_ptr, http::consts::kLocalhost, kPort);

        // dummy components
        server_ptr->AddListener("/count", http::Method::post, &dummy::handlers::handle_count);
        server_ptr->AddListener("/dict-add", http::Method::post, &dummy::handlers::handle_dict_add);
        server_ptr->AddListener("/dict-get", http::Method::get, &dummy::handlers::handle_dict_get);
        
        // dummy http
        server_ptr->AddListener("/get", http::Method::get, &dummy::handlers::handle_get);
        server_ptr->AddListener("/get_parametrized", http::Method::get,
                                &dummy::handlers::handle_get_parametrized);
        server_ptr->AddListener("/ping", http::Method::get, &dummy::handlers::handle_ping);
        server_ptr->AddListener("/post", http::Method::post, &dummy::handlers::handle_post);
        
        server_ptr->Listen();

        io_context_ptr->run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}