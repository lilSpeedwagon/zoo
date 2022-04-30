#include <common/include/utils/errors.hpp>

#include <errno.h>
#include <string.h>

namespace common::utils::errors {

std::string GetLastError() {
    constexpr const size_t kBufferSize = 256;
    char buffer[kBufferSize];
    // In MSVS strerror_s() has a reversed params order.
    // In other compilers it returns resultin buffer ptr.
    #ifdef _MSC_VER
    strerror_s(buffer, kBufferSize, errno);
    #else
    strerror_r(errno, buffer, kBufferSize);
    #endif
    return std::string(buffer);
}

} // namespace common::utils::errors