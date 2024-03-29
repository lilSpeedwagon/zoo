#include <chrono>
#include <string>

#include <boost/regex.hpp>

#include <catch2/catch.hpp>

#include <common/include/logging.hpp>
#include <common/src/logging/logger.hpp>
#include <common/src/logging/sink_string.hpp>

namespace common::tests::log {

namespace {

static constexpr std::string_view kTimePlaceholder = "time";

void CheckSink(std::shared_ptr<common::logging::SinkString> sink_ptr, const char* expected) {
    static const boost::regex kTimeRegex("\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z");
    auto data = sink_ptr->GetString();
    data = boost::regex_replace(data, kTimeRegex, kTimePlaceholder, boost::match_default | boost::format_sed);
    CHECK(data == std::string(expected));
}

class LoggerFixture {
public:
    LoggerFixture() {
        auto& logger = common::logging::LoggerFrontend::GetMainLogger();
        logger.SetLevelFilter(common::logging::LogLevel::Trace);
        logger.SetFlushLevel(common::logging::LogLevel::Error);
        logger.ResetSinks();
        sink_ptr = std::make_shared<common::logging::SinkString>();
        logger.AddSink(sink_ptr);
        logger.SetBufferMaxSize(0);
    }
protected:
    std::shared_ptr<common::logging::SinkString> sink_ptr;
};

} // namespace


TEST_CASE_METHOD(LoggerFixture, "Log without flush", "[Log]") {
    auto& logger = common::logging::LoggerFrontend::GetMainLogger();
    logger.SetBufferMaxSize(10);

    LOG_INFO() << "data";
    CheckSink(sink_ptr, "");
}

TEST_CASE_METHOD(LoggerFixture, "Log with force flush", "[Log]") {
    auto& logger = common::logging::LoggerFrontend::GetMainLogger();
    logger.SetBufferMaxSize(10);

    LOG_ERROR() << "error msg";
    CheckSink(sink_ptr, "ERROR\t[time]\terror msg\n");
}

TEST_CASE_METHOD(LoggerFixture, "Log", "[Log]") {
    LOG_TRACE() << "trace data";
    LOG_DEBUG() << "debug data";
    LOG_INFO() << "info data";
    LOG_WARNING() << "warning data";
    LOG_ERROR() << "error data";
    CheckSink(sink_ptr, "TRACE\t[time]\ttrace data\n"
                        "DEBUG\t[time]\tdebug data\n"
                        "INFO\t[time]\tinfo data\n"
                        "WARNING\t[time]\twarning data\n"
                        "ERROR\t[time]\terror data\n");
} 

TEST_CASE_METHOD(LoggerFixture, "Log level", "[Log]") {
    auto& logger = common::logging::LoggerFrontend::GetMainLogger();
    logger.SetLevelFilter(common::logging::LogLevel::Warning);

    LOG_DEBUG() << "debug data";
    LOG_INFO() << "info data";
    LOG_WARNING() << "warning data";
    LOG_ERROR() << "error data";
    CheckSink(sink_ptr, "WARNING\t[time]\twarning data\n"
                        "ERROR\t[time]\terror data\n");
}

TEST_CASE_METHOD(LoggerFixture, "Log msg max length", "[Log]") {
    auto& logger = common::logging::LoggerFrontend::GetMainLogger();
    logger.SetMessageMaxSize(5);

    LOG_DEBUG() << "lorem ipsum";
    CheckSink(sink_ptr, "DEBUG\t[time]\tlorem... (6 more chars)\n");
}

} // namespace common::tests::log