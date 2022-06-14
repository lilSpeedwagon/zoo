#include <common/include/components_engine.hpp>

#include <common/include/format.hpp>
#include <common/include/logging.hpp>

namespace common::components {

ComponentsEngine::ComponentsEngine() 
    : is_initialized_{false} {}
    
ComponentsEngine::~ComponentsEngine() {}

ComponentsEngine& ComponentsEngine::GetInstance() {
    static ComponentsEngine engine{};
    return engine;
}

void ComponentsEngine::Init() {
    if (is_initialized_) {
        return;
    }
    
    LOG_DEBUG() << "Init components system";
    for (auto [name, component_ptr] : components_) {
        LOG_DEBUG() << "Init component " << name;
        component_ptr->Init();
    }
    is_initialized_ = true;
    LOG_DEBUG() << "Components system is ready";
}

void ComponentsEngine::Register(ComponentPtr component_ptr) {
    auto name = std::string(component_ptr->Name());
    components_[std::move(name)] = component_ptr;
}

void ComponentsEngine::Reset() {
    components_.clear();
    is_initialized_ = false;
}

} // namespace common::components