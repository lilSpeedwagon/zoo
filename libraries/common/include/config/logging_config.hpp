#pragma once

#include <common/include/config.hpp>
#include <common/include/json.hpp>
#include <common/include/logging.hpp>

namespace common::config {

/// @brief Returns log config loaded by specified path
/// or default config if nothing was found.
logging::LogSettings GetLogConfig(const std::string& path = ".\\log_config.json");

} // namespace common::config