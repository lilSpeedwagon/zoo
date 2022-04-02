#include <common/include/utils/errors.hpp>

namespace common::utils::errors {

std::string GetLastError() {
    constexpr const size_t kBufferSize = 256;
    char buffer[kBufferSize];
    strerror_s(buffer, kBufferSize, errno);
    return std::string(buffer);
}

} // namespace common::utils::errors