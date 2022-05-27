#pragma once

#include <memory>
#include <string>

namespace common::components {

/// @class Base class for the pieces of components system.
class ComponentBase {
public:
    static const std::string kName;

    ComponentBase() : is_ready_{false} {}
    virtual ~ComponentBase() {}

    bool IsReady() const { return is_ready_; }
    virtual void Init() = 0;
    virtual std::string Name() const = 0;

protected:
    bool is_ready_;
};

using ComponentPtr = std::shared_ptr<ComponentBase>;

} // namespace common::components