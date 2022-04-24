#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace common::utils::algo {

template<typename ContainerT>
inline typename ContainerT::mapped_type GetOrDefault(
    const ContainerT& container, const typename ContainerT::key_type& key,
    const typename ContainerT::mapped_type& default) {
    if (const auto it = container.find(key);
        it != container.cend()) {
        return it->second;
    }
    return default;
}

template<typename ContainerT>
inline std::optional<typename ContainerT::mapped_type> GetOptional(
    const ContainerT& container, const typename ContainerT::key_type& key) {
    if (const auto it = container.find(key);
        it != container.cend()) {
        return it->second;
    }
    return std::nullopt;
}

inline std::string ToLower(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](const auto c) { return std::tolower(c); });
    return result;
}

inline std::string ToUpper(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](const auto c) { return std::toupper(c); });
    return result;
}

} // namespace common::utils::algo