#include <common/include/format.hpp>

// localtime_s
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>

#include <exception>

namespace common::format {

std::string TimePointToString(
    const std::chrono::system_clock::time_point& timepoint,
    const std::string_view& format) {
    auto raw_time = std::chrono::system_clock::to_time_t(timepoint);

    struct tm time_buffer;

    // In MSVS localtime_s() has a reversed params order and returns errno.
    // In other compilers it returns resultin buffer ptr.
    #ifdef _MSC_VER
    auto err = localtime_s(&time_buffer, &raw_time);
    if (err) {
        throw std::runtime_error("Bad localtime conversion");
    }
    #else
    auto result_ptr = localtime_s(&raw_time, &time_buffer);
    if (!result_ptr) {
        throw std::runtime_error("Bad localtime conversion");
    }
    #endif

    constexpr size_t kBufferSize = 256;
    char buffer[kBufferSize];
    const auto result = strftime(buffer, kBufferSize, format.data(), &time_buffer);
    if (!result) {
        throw std::runtime_error("Bad timepoint format");
    }
    return std::string(buffer);
}

}