#include "logger.hpp"

#include <time.h>

namespace common::logging {

namespace {

std::string FormatLogLevel(const LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
    }
}

std::string FormatLog(LogMsg&& msg) {
    constexpr std::string_view kTimeFormat = "%F %T";

    std::stringstream ss{};
    ss << FormatLogLevel(msg.level);
    ss << "\t[" << format::TimePointToString(msg.timepoint, kTimeFormat) << "]\t";
    ss << msg.message;
    return ss.str();
}

bool IsEmpty(const std::stringstream& ss) {
    return ss.rdbuf()->in_avail() == 0;
}

} // namespace

Logger::Logger() {
    level_filter_ = LogLevel::Debug;
    buffer_max_size_ = 100;
}

void Logger::Log(LogMsg&& msg) {
    std::lock_guard lock(buffer_mutex_);
    buffer_.push(std::move(msg));
    if (buffer_.size() > buffer_max_size_) {
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
                ss << FormatLog(std::move(msg)) << '\n';
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

} // namespace common::logging