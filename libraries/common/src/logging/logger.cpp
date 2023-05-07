#include "logger.hpp"

#include <string>
#include <time.h>

#include <common/include/format.hpp>

namespace common::logging {

namespace {

std::string FormatLogLevel(const LogLevel level) {
    switch (level) {
        case LogLevel::Trace:
            return "TRACE";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            throw std::logic_error(common::format::Format(
                "Invalid log level {}", static_cast<int>(level)));
    }
}

std::string FormatLog(LogMsg&& msg, size_t limit) {
    constexpr std::string_view kTimeFormat = "%FT%TZ";

    std::stringstream ss{};
    ss << FormatLogLevel(msg.level);
    ss << "\t[" << format::TimePointToString(msg.timepoint, kTimeFormat) << "]\t";
    ss << common::format::ShrinkString(msg.message, limit);
    return ss.str();
}

bool IsEmpty(const std::stringstream& ss) {
    return ss.rdbuf()->in_avail() == 0;
}

} // namespace

Logger::Logger() {
    auto default_settings = LogSettings{};
    level_filter_ = default_settings.log_level;
    buffer_max_size_ = default_settings.buffer_max_size;
    msg_max_size_ = default_settings.msg_max_size;
}

void Logger::Log(LogMsg&& msg) {
    const bool flush = msg.level >= flush_level_;
    std::lock_guard lock(buffer_mutex_);
    buffer_.push(std::move(msg));
    if (flush || buffer_.size() > buffer_max_size_) {
        Flush();
    }
}

void Logger::Flush() {
    if (sinks_.empty()) {
        Clear();
        return;
    }

    std::stringstream ss{};
    {
        std::lock_guard lock(buffer_mutex_);
        if (buffer_.empty()) {
            return;
        }

        while (!buffer_.empty()) {
            auto& msg = buffer_.front();
            if (msg.level >= level_filter_) {
                ss << FormatLog(std::move(msg), msg_max_size_) << '\n';
            }
            buffer_.pop();
        }
    }

    for (const auto sink_ptr : sinks_) {
        sink_ptr->Write(ss);
    }
}

void Logger::Clear() {
    std::lock_guard lock(buffer_mutex_);
    while (!buffer_.empty()) {
        buffer_.pop();
    }
}

void Logger::SetLevelFilter(LogLevel level) {
    level_filter_ = level;
}

void Logger::SetFlushLevel(LogLevel level) {
    flush_level_ = level;
}

void Logger::AddSink(const LoggerSinkPtr sink) {
    sinks_.push_back(sink);
}

void Logger::ResetSinks() {
    Flush();
    sinks_.clear();
}

void Logger::SetBufferMaxSize(size_t size) {
    buffer_max_size_ = size;
}

size_t Logger::GetBufferMaxSize() const {
    return buffer_max_size_;
}

void Logger::SetMessageMaxSize(size_t size) {
    msg_max_size_ = size;
}

size_t Logger::GetMessageMaxSize() const {
    return msg_max_size_;
}

} // namespace common::logging