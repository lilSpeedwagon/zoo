#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace common::utils::algo {

template<typename ContainerT, typename ValueT = ContainerT::mapped_type,
         typename KeyT = ContainerT::key_type>
ValueT GetOrDefault(
    const ContainerT& container, const KeyT& key, const ValueT& default) {
    if (const auto it = container.find(key);
        it != container.cend()) {
        return it->second;
    }
    return default;
}

template<typename ContainerT, typename ValueT = ContainerT::mapped_type,
         typename KeyT = ContainerT::key_type>
std::optional<ValueT> GetOptional(const ContainerT& container, const KeyT& key) {
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