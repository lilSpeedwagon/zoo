#include <common/include/component_base.hpp>

namespace common::components {

ComponentBase::ComponentBase() : is_ready_{false} {}
ComponentBase::~ComponentBase() {}

bool ComponentBase::IsReady() const {
    return is_ready_;
}

} // namespace common::components