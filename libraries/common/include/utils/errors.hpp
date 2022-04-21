#pragma once

#include <string>

namespace common::utils::errors {

/// @brief Returns string representation
/// of errno in current system locale.
std::string GetLastError();

} // namespace common::utils::errors