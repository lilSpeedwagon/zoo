#pragma once

#include <type_traits>
#include <string>
#include <sstream>

namespace common::format {

constexpr std::string_view kPlaceholder = "{}";

template<typename T>
std::string ToString(const T& value) {
    return std::to_string(value);
}

template<>
inline std::string ToString<bool>(const bool& value) {
    return value ? "true" : "false";
}

template<>
inline std::string ToString<std::string>(const std::string& value) {
    return value;
}

template<>
inline std::string ToString<std::string_view>(const std::string_view& value) {
    return std::string(value);
}

template<>
inline std::string ToString<std::exception>(const std::exception& value) {
    std::stringstream ss{};
    ss << "(" << typeid(value).name() << ") " << value.what();
    return ss.str();
}

template<typename ...Args>
std::string Format(const std::string& format, Args&& ...args) {
    auto format_copy = format;
    return impl::FormatImpl(std::move(format_copy), std::forward<Args>(args)...);
}

namespace impl {

inline std::string FormatImpl(std::string&& format) {
    const auto placeholder_index = format.find(kPlaceholder);
    if (placeholder_index != std::string::npos) {
        throw std::logic_error("not enough arguments in Format() call");
    }
    return std::move(format);
}

template<typename T, typename ...Args>
std::string FormatImpl(std::string&& format, T arg, Args&& ...args) {
    const auto placeholder_index = format.find(kPlaceholder);
    if (placeholder_index == std::string::npos) {
        throw std::logic_error("extra arguments in Format() call");
    }
    format.replace(placeholder_index, kPlaceholder.size(), ToString(arg));
    return FormatImpl(std::move(format), std::forward<Args>(args)...);
}

} // namespace impl

} // namespace common::format