#include <components/include/components_engine.hpp>

#include <optional>

#include <common/include/format.hpp>
#include <common/include/logging.hpp>

namespace components {

ComponentsEngine::ComponentsEngine() : is_initialized_{false}, components_{} {}

ComponentsEngine::~ComponentsEngine() {}

ComponentsEngine& ComponentsEngine::GetInstance() {
    static ComponentsEngine engine{};
    return engine;
}

void ComponentsEngine::Register(ComponentPtr component_ptr) {
    auto name = std::string(component_ptr->Name());
    if (components_.count(name) != 0) {
        throw std::logic_error(
            common::format::Format("component '{}' already registered", name));
    }
    components_[std::move(name)] = component_ptr;
}

ComponentPtr ComponentsEngine::Get(const char* name) const {
    if (auto it = components_.find(name);
        it != components_.cend()) {
        return it->second;
    }
    return nullptr;
}

void ComponentsEngine::Clear() {
    components_.clear();
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
    LOG_INFO() << "Components system is ready";
}

void ComponentsEngine::Reset() {
    for (auto& component : components_) {
        LOG_DEBUG() << "Reset component " << component.second->kName;
        component.second->Reset();
    }
    LOG_INFO() << "Components system was reset";
}

} // namespace components