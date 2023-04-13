#include <iostream>

#include <common/include/config/logging_config.hpp>
#include <common/include/logging.hpp>
#include <common/include/thread_pool.hpp>
#include <components/include/components_controller.hpp>
#include <components/include/components_engine.hpp>
#include <http/include/consts.hpp>
#include <http/include/default_handlers.hpp>
#include <http/include/http_server.hpp>

#include <components/storage.hpp>
#include <handlers/clear.hpp>
#include <handlers/create.hpp>
#include <handlers/delete.hpp>
#include <handlers/get.hpp>
#include <handlers/list.hpp>
#include <handlers/update.hpp>


namespace {

static constexpr unsigned short kPort = 5555;

inline std::string MakePath(const char* method) {
    static constexpr const char* kApiPrefix = "/api/v1/documents/{}";
    return common::format::Format(kApiPrefix, method);
}

common::logging::LoggerController InitLogger() {
    auto log_config = common::config::GetLogConfig();
    common::logging::LoggerController controller(log_config);
    LOG_DEBUG() << "Logger is ready";
    return controller;
}

auto InitComponents() {
    auto storage_ptr = std::make_shared<documents::components::Storage>();
    auto& engine = components::ComponentsEngine::GetInstance();
    engine.Register(storage_ptr);
    engine.Init();

    auto controller_ptr = std::make_shared<components::ComponentsController>();
    controller_ptr->RunAsync();
    return controller_ptr;
}

} // namespace

int main() {
    const size_t kThreadsCount = 4;

    try {
        const auto log_controller = InitLogger();
        const auto components_controller_ptr = InitComponents();

        LOG_INFO() << "Setting up the server...";
        common::threading::IoThreadPool pool(kThreadsCount);
        auto server_ptr = std::make_shared<http::server::HttpServer>(
            pool.GetContextPtr(), http::consts::kLocalhost, kPort);
        server_ptr->AddListener("/ping", http::Method::get, &http::handlers::handle_ping);
        server_ptr->AddListener(MakePath("clear"), http::Method::post, &documents::handlers::HandleClear);
        server_ptr->AddListener(MakePath("create"), http::Method::post, &documents::handlers::handle_create);
        server_ptr->AddListener(MakePath("delete"), http::Method::post, &documents::handlers::handle_delete);
        server_ptr->AddListener(MakePath("get"), http::Method::get, &documents::handlers::handle_get);
        server_ptr->AddListener(MakePath("list"), http::Method::get, &documents::handlers::handle_list);
        server_ptr->AddListener(MakePath("update"), http::Method::post, &documents::handlers::handle_update);

        server_ptr->Listen();
        pool.RunInThisThread();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}