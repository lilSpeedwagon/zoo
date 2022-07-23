#include <components/include/components_controller.hpp>

#include <common/include/json.hpp>
#include <common/include/logging.hpp>
#include <http/include/consts.hpp>
#include <http/include/models.hpp>
#include <http/include/utils.hpp>

#include <components_engine.hpp>

namespace components {

namespace {

static const size_t kControlThreadsCount = 1;
static const char* kDefaultContolServerAddress = "127.0.0.1";
static const size_t kDefaultControlServerPort = 5050;

struct ResetRequest {
    std::optional<std::string> component_name{};
};

void from_json(const common::json::json& data, ResetRequest& request) {
    if (data.contains("component_name")) {
        data.at("component_name").get_to(request.component_name);
    }
}

http::Response HandleReset(http::Request&& request) {
    const auto reset_request = http::utils::Parse<ResetRequest>(request);
    auto& components = ComponentsEngine::GetInstance();
    if (reset_request.component_name.has_value()) {
        const auto& name = reset_request.component_name.value();
        auto component_ptr = components.Get(name.c_str());
        if (component_ptr != nullptr) {
            component_ptr->Reset();
        } else {
            throw http::exceptions::NotFound(
                common::format::Format("Component \'{}\' not found", name).c_str());
        }
    } else {
        components.Reset();
    }
    return http::Response(http::Status::ok, http::consts::kVersion);
}

} // namespace

ComponentsController::ComponentsController() 
    : is_running_(false), pool_(kControlThreadsCount),
      server_ptr_(std::make_shared<http::server::HttpServer>(
        pool_.GetContextPtr(), kDefaultContolServerAddress, kDefaultControlServerPort)) {
    server_ptr_->AddListener("/test-control/reset", http::Method::post, &HandleReset);
}

ComponentsController::~ComponentsController() {}

void ComponentsController::RunAsync() {
    LOG_INFO() << "Running ComponentsController";
    is_running_ = false;
    server_ptr_->Listen();
    pool_.Run();
}

void ComponentsController::Stop() {
    LOG_INFO() << "Stopping ComponentsController";
    pool_.Stop();
    is_running_ = false;
}

bool ComponentsController::IsRunning() const {
    return is_running_;
}

} // namespace components