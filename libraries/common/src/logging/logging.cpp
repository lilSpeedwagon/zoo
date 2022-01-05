#include "logging.hpp"

#include <boost/log/core.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace common::logging {

namespace {

constexpr size_t kDefaultLogRotateSize = 10 * 1024 * 1024;  // 10 Mb
constexpr std::string_view kDefaultLogFormat = "[%TimeStamp%]: %Message%";
static const boost::filesystem::path kDefaultFileNamePattern = "log_%N.log";

namespace keywords = boost::log::keywords;

} // namespace

void InitLog() {
    boost::log::add_file_log(
        keywords::file_name = kDefaultFileNamePattern,
        keywords::rotation_size = kDefaultLogRotateSize,
        keywords::time_based_rotation = 
            boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // rotate at midnight
        keywords::format = kDefaultLogFormat
    );

    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::debug
    );
}

} // namespace common::logging