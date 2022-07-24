#include <iostream>

#include <common/include/config/logging_config.hpp>
#include <common/include/format.hpp>
#include <common/include/logging.hpp>
#include <common/include/thread_pool.hpp>
#include <components/include/components_controller.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/consts.hpp>
#include <http/include/default_handlers.hpp>
#include <http/include/http_server.hpp>

#include <components/api_storage.hpp>
#include <handlers/create.hpp>
#include <handlers/delete.hpp>
#include <handlers/get.hpp>
#include <handlers/list.hpp>
#include <handlers/update.hpp>

static constexpr unsigned short kPort = 80;

common::logging::LoggerController InitLogger() {
    auto log_config = common::config::GetLogConfig();
    common::logging::LoggerController controller(log_config);
    LOG_DEBUG() << "Logger is ready";
    return controller;
}

auto InitComponents() {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto api_storage_ptr = 
        std::make_shared<api_config::components::ApiConfigStorage>();
    engine.Register(api_storage_ptr);
    engine.Init();

    auto controller_ptr = std::make_shared<components::ComponentsController>();
    controller_ptr->RunAsync();
    return controller_ptr;
}

int main() {
    const size_t kThreadsCount = 4;

    try {
        const auto log_controller = InitLogger();
        const auto components_controller_ptr = InitComponents();

        LOG_INFO() << "Setting up the server...";
        common::threading::IoThreadPool pool(kThreadsCount);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            pool.GetContextPtr(), http::consts::kLocalhost, kPort);
        server_ptr->AddListener("/ping", http::Method::get,
                                &http::handlers::handle_ping);
        server_ptr->AddListener("/api/v1/api-config/create", http::Method::post,
                                &api_config::handlers::handle_create);
        server_ptr->AddListener("/api/v1/api-config/delete", http::Method::post,
                                &api_config::handlers::handle_delete);
        server_ptr->AddListener("/api/v1/api-config/get", http::Method::get,
                                &api_config::handlers::handle_get);
        server_ptr->AddListener("/api/v1/api-config/list", http::Method::get,
                                &api_config::handlers::handle_list);
        server_ptr->AddListener("/api/v1/api-config/update", http::Method::post,
                                &api_config::handlers::handle_update);
        server_ptr->Listen();
        pool.RunInThisThread();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}