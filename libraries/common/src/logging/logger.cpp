#include "logger.hpp"

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
    std::stringstream ss{};
    ss << FormatLogLevel(msg.level);
    ss << " [" << msg.timepoint << "] ";
    ss << msg.message << '\n';
    return ss.str();
}

} // namespace

void Logger::Log(LogMsg&& msg) {
    std::lock_guard lock(buffer_mutex_);
    buffer_.push(std::move(msg));
    if (buffer_.size() > buffer_max_size_) {
        Flush();
    }
}

void Logger::AddSink(const LoggerSinkPtr sink) {
    sinks_.push_back(sink);
}

void Logger::Flush() {
    std::stringstream ss{};
    {
        std::lock_guard lock(buffer_mutex_);
        while (!buffer_.empty()) {
            auto& msg = buffer_.front();
            ss << FormatLog(std::move(msg)) << '\n';
            buffer_.pop();
        }
    }
    for (const auto& sink_ptr : sinks_) {
        sink_ptr->Write(ss);
    }
}

void Logger::Clear() {
    std::lock_guard lock(buffer_mutex_);
    while (!buffer_.empty()) {
        buffer_.pop();
    }
}

} // namespace common::logging