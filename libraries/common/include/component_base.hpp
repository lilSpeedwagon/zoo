#pragma once

#include <memory>
#include <string>

namespace common::components {

/// @class Base class for the pieces of components system.
class ComponentBase {
public:
    /// Unique name of the component.
    /// Must be overriden in the inherited class.
    static constexpr const char* kName = "base";

    ComponentBase();
    virtual ~ComponentBase();

    bool IsReady() const;
    virtual const char* Name() const = 0;
    virtual void Init() = 0;

protected:
    bool is_ready_;
};

using ComponentPtr = std::shared_ptr<ComponentBase>;

} // namespace common::components