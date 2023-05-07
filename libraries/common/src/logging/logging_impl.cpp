#include <common/include/logging.hpp>

#include <iostream>
#include <pthread.h>

#include <common/src/logging/logger.hpp>
#include <common/src/logging/sink_stdout.hpp>
#include <common/src/logging/sink_fs.hpp>

namespace common::logging {

LogHolder::LogHolder(LogLevel level, std::string&& module_name) 
    : log_entry_{
        "",                                 // message
        level,                              // level
        std::this_thread::get_id(),         // thread_id
        std::chrono::system_clock::now(),   // timepoint
        std::move(module_name),             // module
    } {}

LogHolder::~LogHolder() {
    try {
        LoggerFrontend::GetMainLogger().Log(std::move(log_entry_));
    } catch(const std::exception& ex) {
        std::cerr << "~LogHolder() exception thrown: " << ex.what() << '\n';
    }
}

Logger& LoggerFrontend::GetMainLogger() {
    static Logger logger{};
    return logger;
}

LoggerController::LoggerController(const LogSettings& settings)
    : is_enabled_{false} {
    Reconfigure(settings);
}

LoggerController::LoggerController(LoggerController&& controller) {
    settings_ = std::move(controller.settings_);
    control_thread_ = std::move(controller.control_thread_);
    is_enabled_ = controller.is_enabled_;
    controller.is_enabled_ = false;
}

LoggerController::~LoggerController() {
    StopControlThread();
}

void LoggerController::Reconfigure(const LogSettings& settings) {
    StopControlThread();
    settings_ = settings;
    
    // todo lock logger
    auto& logger = LoggerFrontend::GetMainLogger();
    logger.SetLevelFilter(settings.log_level);
    logger.SetBufferMaxSize(settings.buffer_max_size);
    logger.SetMessageMaxSize(settings.msg_max_size);
    logger.SetLevelFilter(settings.log_level);
    logger.SetFlushLevel(settings.flush_level);
    if (settings.log_to_stdout) {
        logger.AddSink(std::make_shared<SinkStdout>());
    }
    logger.AddSink(std::make_shared<SinkFS>(settings.path,
                                            settings.file_prefix));
    RunControlThread();
}

void LoggerController::RunControlThread() {
    is_enabled_ = true;
    control_thread_ = std::thread(
        [&enabled = is_enabled_](std::chrono::milliseconds flush_delay) {
        try {
            while (enabled) {
                auto& logger = LoggerFrontend::GetMainLogger();
                logger.Flush();
                std::this_thread::sleep_for(flush_delay);
            }
        } catch (const std::exception& ex) {
            std::cerr << "exception in log control thread: " << ex.what() << '\n';
            throw;
        }
    }, settings_.flush_delay);
}

void LoggerController::StopControlThread() {
    if (is_enabled_) {
        is_enabled_ = false;
        control_thread_.join();
    }
}

} // namespace common::logging