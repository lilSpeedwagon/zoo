#include <components/include/component_base.hpp>

namespace components {

ComponentBase::ComponentBase() : is_ready_{false} {}
ComponentBase::~ComponentBase() {}

bool ComponentBase::IsReady() const {
    return is_ready_;
}

} // namespace components