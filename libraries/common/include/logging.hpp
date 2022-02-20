#pragma once

#include <chrono>
#include <string>
#include <thread>

#include <boost/chrono.hpp>

#include <common/include/format.hpp>
#include <common/include/utils.hpp>

namespace common::logging {

/// @class Logger forward declaration.
class Logger;

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
};

/// @struct a single log entry with utility data.
struct LogMsg {
    std::string message{};
    LogLevel level{};
    std::thread::id thread_id{};
    std::chrono::system_clock::time_point timepoint{};
    std::string module{};
};

/// @class the singletone entrypoint to access main logger.
class LoggerFrontend final {
public:
    static Logger& GetMainLogger();
private:
    LoggerFrontend() = default;
    ~LoggerFrontend() = default;
    LoggerFrontend(const LoggerFrontend&) = delete;
    LoggerFrontend& operator=(const LoggerFrontend&) = delete;
};

/// @class utility class for stream-like logging.
/// Create the entry of the class as a temporary object
/// and all consumed log data will be written to logger in the 
/// end of the entry lifetime.
/// Consider to use this class via LOG() defines insted of a manual usage.
class LogHolder {
public:
    LogHolder(LogLevel level, std::string&& module_name);
    ~LogHolder();

    template<typename T>
    LogHolder& operator<<(const T& msg) {
        log_entry_.message += format::ToString(msg);
        return *this;
    }

private:
    LogHolder(const LogHolder&) = delete;
    LogHolder& operator=(const LogHolder&) = delete;

    LogMsg log_entry_;
};

#ifdef _MSC_VER
    #define LOG(lvl) common::logging::LogHolder((lvl), __FUNCSIG__)
#else
    #define LOG(lvl) common::logging::LogHolder((lvl), __PRETTY_FUNCTION__)
#endif

/// @brief macros for stream like logging.
/// Example: LOG_INFO() << "log message";
#define LOG_DEBUG() LOG(common::logging::LogLevel::Debug)
#define LOG_INFO() LOG(common::logging::LogLevel::Info)
#define LOG_WARNING() LOG(common::logging::LogLevel::Warning)
#define LOG_ERROR() LOG(common::logging::LogLevel::Error)


/// @brief Init main logger with the default sinks (stdout).
/// TODO: spawn a new thread to flush log asynchronously.
/// TODO: add log configuration for sinks, log format, min log level, etc.
void InitMainLogger();

} // namespace common::logging
