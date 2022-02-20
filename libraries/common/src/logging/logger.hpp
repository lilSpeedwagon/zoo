#pragma once

#include <mutex>
#include <queue>

#include <common/include/logging.hpp>
#include <common/src/logging/sink_base.hpp>

namespace common::logging {

/// @class the main logger.
class Logger final {
public:
    Logger();

    void Log(LogMsg&& msg);
    void AddSink(const LoggerSinkPtr sink);
    void SetLevelFilter(LogLevel level);
    void Flush();
    void Clear();

private:
    std::vector<LoggerSinkPtr> sinks_;
    size_t buffer_max_size_;
    std::recursive_mutex buffer_mutex_;
    std::queue<LogMsg> buffer_;
    LogLevel level_filter_;
};

} // namespace common::logging