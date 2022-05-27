#pragma once

#include <unordered_map>

#include <common/include/component_base.hpp>
#include <common/include/format.hpp>
#include <common/include/logging.hpp>

namespace common::components {

/// @class Global component system 
/// to access registered components by their's type.
/// Note: there can be only one component of each type.
class ComponentsEngine {
public:
    static ComponentsEngine& GetInstance();
    ~ComponentsEngine();

    /// @brief Init all of the registered components.
    void Init();

    /// @brief Register a new component.
    void Register(ComponentPtr component_ptr);

    /// @brief Remove all of the registered components.
    void Reset();

    /// @brief Get a component of the specified type.
    /// @throws std::logic_error if such a component is not registered.
    template<typename T, 
             typename = std::enable_if<std::is_base_of<ComponentBase, T>::value> >
    std::shared_ptr<T> Get() {
        const auto name = T::kName;
        if (auto it = components_.find(name);
            it != components_.cend()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        throw std::logic_error(
                format::Format("component '{}' is not registered", name));
    }

private:
    ComponentsEngine();

    bool is_initialized_;
    std::unordered_map<std::string, ComponentPtr> components_;
};

} // namespace common::components