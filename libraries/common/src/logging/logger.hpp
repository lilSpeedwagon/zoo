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
    void SetLevelFilter(LogLevel level);
    void SetFlushLevel(LogLevel level);
    void Flush();
    void Clear();

    void AddSink(const LoggerSinkPtr sink);
    void ResetSinks();

    void SetBufferMaxSize(size_t size);
    size_t GetBufferMaxSize() const;

    void SetMessageMaxSize(size_t size);
    size_t GetMessageMaxSize() const;

private:
    std::vector<LoggerSinkPtr> sinks_;
    size_t buffer_max_size_;
    std::recursive_mutex buffer_mutex_;
    std::queue<LogMsg> buffer_;
    LogLevel level_filter_;
    LogLevel flush_level_;
    size_t msg_max_size_;
};

} // namespace common::logging