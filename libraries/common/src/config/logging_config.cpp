#include <common/include/config/logging_config.hpp>

#include <exception>

#include <common/include/format.hpp>
#include <common/include/utils/algo.hpp>

namespace common {

namespace {

template<typename T>
T GetOrDefault(const common::json::json& data, const std::string& key,
               const T& default) {
    if (const auto it = data.find(key);
        it != data.cend()) {
        return it->get<T>();
    }
    else {
        return default;
    }
}

logging::LogLevel ParseLogLevel(const std::string& log_level) {
    static const std::unordered_map<std::string, logging::LogLevel> levels = {
        {"debug", logging::LogLevel::Debug},
        {"info", logging::LogLevel::Info},
        {"warning", logging::LogLevel::Warning},
        {"error", logging::LogLevel::Error},
    };

    if (const auto it = levels.find(utils::algo::ToLower(log_level));
        it != levels.cend()) {
        return it->second;
    }
    throw std::runtime_error(
        format::Format("invalid log level '{}'", log_level));
}

logging::LogSettings GetLogConfigImpl(const std::string& path) {
    config::Config config{};
    try {
        config = config::Config::FromFile(path);
    } catch(const std::runtime_error&) {
        return logging::LogSettings{};
    }
    return config.Get<logging::LogSettings>();
}

} // namespace

namespace logging {

void from_json(const json::json& data, logging::LogSettings& config) {
    auto flush_delay = GetOrDefault<int>(data, "flush_delay", 100);
    auto log_level = GetOrDefault<std::string>(data, "log_level", "Debug");
    auto buffer_max_size = GetOrDefault<int>(data, "buffer_max_size", 1024);
    auto log_to_stdout = GetOrDefault<bool>(data, "log_to_stdout", true);
    auto path = GetOrDefault<std::string>(data, "path", ".\\");
    auto file_prefix = GetOrDefault<std::string>(data, "file_prefix", "log_");

    config.flush_delay = static_cast<std::chrono::milliseconds>(flush_delay);
    config.log_level = ParseLogLevel(log_level);
    config.buffer_max_size = static_cast<size_t>(buffer_max_size);
    config.log_to_stdout = log_to_stdout;
    config.path = std::move(path);
    config.file_prefix = std::move(path);
}

} // nemspace logging

namespace config {

logging::LogSettings GetLogConfig(const std::string& path) {
    try {
        return GetLogConfigImpl(path);
    } catch (const std::exception& ex) {
        throw std::runtime_error(format::Format("Cannot load log config: {}", ex.what()));
    }
}
    
} // namespace config

} // namespace common
