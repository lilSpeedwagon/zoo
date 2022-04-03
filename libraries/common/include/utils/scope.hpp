#pragma once

#include <functional>

namespace common::utils::scope {

/// @class Scope guard invokes callback when it will
/// reach the borders of the current scope.
template<typename T>
class ScopeGuard final {
public:
    ScopeGuard(const std::function<T>& callback) 
        : callback_(callback) {}
    ~ScopeGuard() {
        if (callback_) {
            callback_();
        }
    }
private:
    ScopeGuard(const ScopeGuard&);
    ScopeGuard& operator=(const ScopeGuard&);
    std::function<T> callback_;
};

} // namespace common::utils::scope