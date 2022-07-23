#pragma once

#include <unordered_map>

#include <boost/core/noncopyable.hpp>

#include <common/include/format.hpp>

#include <components/include/component_base.hpp>

namespace components {

/// @class Global component system 
/// to access registered components by their's type.
/// Note: there can be only one component of each type.
class ComponentsEngine : private boost::noncopyable {
public:
    static ComponentsEngine& GetInstance();
    ~ComponentsEngine();

    /// @brief Register a new component.
    void Register(ComponentPtr component_ptr);

    /// @brief Remove all of the registered components.
    void Clear();
    
    /// @brief Init all of the registered components.
    void Init();

    /// @brief Reset all of the registered components.
    void Reset();

    /// @brief Get a component of the specified type.
    /// @throws std::logic_error if such a component is not registered.
    template<typename T>
    std::shared_ptr<
        typename std::enable_if<std::is_base_of<ComponentBase, T>::value, T>::type
    > Get() {
        const auto name = T::kName;
        if (auto it = components_.find(name);
            it != components_.cend()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        throw std::logic_error(
            common::format::Format("component '{}' is not registered", name));
    }

    /// @brief Get a component by name.
    /// @returns Pointer to the requested component or empty pointer if not found.
    ComponentPtr Get(const char* name) const;

private:
    ComponentsEngine();

    bool is_initialized_;
    std::unordered_map<std::string, ComponentPtr> components_;
};

} // namespace components