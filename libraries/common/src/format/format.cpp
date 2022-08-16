#include <common/include/format.hpp>

#include <exception>
#include <time.h>

namespace common::format {

namespace {

constexpr std::string_view kShrinkedStringPlaceholder = "... ({} more chars)";
constexpr size_t placeholder_index = kShrinkedStringPlaceholder.find(kPlaceholder);

std::string GetShrinkedPlaceholder(int shrinked_size) {
    std::string result(kShrinkedStringPlaceholder);
    result.replace(placeholder_index, kPlaceholder.size(),
                   std::to_string(shrinked_size));
    return result;
}

} // namespace

std::string TimePointToString(
    const std::chrono::system_clock::time_point& timepoint,
    const std::string_view& format) {
    auto raw_time = std::chrono::system_clock::to_time_t(timepoint);
    tm* time_data_ptr = std::gmtime(&raw_time);
    if (time_data_ptr == nullptr) {
        throw std::runtime_error("Bad timepoint convertation");
    }
    constexpr size_t kBufferSize = 256;
    char buffer[kBufferSize];
    const auto result = 
        strftime(buffer, kBufferSize, format.data(), time_data_ptr);
    if (!result) {
        throw std::runtime_error("Bad timepoint format");
    }
    return std::string(buffer);
}

std::string ShrinkString(const std::string& str, size_t limit) {
    const int shrinked_size = str.size() - limit;
    if (shrinked_size <= 0) {
        return str;
    }

    auto placeholder = GetShrinkedPlaceholder(shrinked_size);

    std::string result{};
    result.reserve(limit + placeholder.size());
    auto end_it = str.cbegin() + limit;
    std::copy(str.cbegin(), end_it, std::back_inserter(result));
    result += placeholder;
    return result;
}

} // namespace common::format