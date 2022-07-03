#pragma once

#include <memory>
#include <string>

namespace components {

/// @class Base class for the pieces of components system.
class ComponentBase {
public:
    /// Unique name of the component.
    /// Must be overriden in the inherited class.
    static constexpr const char* kName = "base";

    ComponentBase();
    virtual ~ComponentBase();

    /// @brief Returns true if component is initialized.
    bool IsReady() const;

    /// @brief Override this method to initialize component.
    virtual void Init() = 0;

    /// @brief Override this method to reset component state.
    virtual void Reset() = 0;
    
    virtual const char* Name() const = 0;

protected:
    bool is_ready_;
};

using ComponentPtr = std::shared_ptr<ComponentBase>;

} // namespace components