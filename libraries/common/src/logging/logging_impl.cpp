#include <common/include/logging.hpp>

#include <iostream>

#include <common/src/logging/logger.hpp>
#include <common/src/logging/sink_stdout.hpp>

namespace common::logging {

LogHolder::LogHolder(LogLevel level, std::string&& module_name) 
    : log_entry_{
        "",                                 // message
        level,                              // level
        std::this_thread::get_id(),         // thread_id
        boost::chrono::system_clock::now(), // timepoint
        std::move(module_name),             // module
    } {}

LogHolder::~LogHolder() {
    try {
        LoggerFrontend::GetMainLogger().Log(std::move(log_entry_));
    } catch(const std::exception& ex) {
        std::cerr << "~LogHolder() exception thrown: " << ex.what();
    }
}

Logger& LoggerFrontend::GetMainLogger() {
    static Logger logger{};
    return logger;
}

void InitMainLogger() {
    auto& logger = LoggerFrontend::GetMainLogger();
    logger.AddSink(std::make_shared<SinkStdout>());
    LOG_DEBUG() << "Main logger is ready";
}

} // namespace common::logging