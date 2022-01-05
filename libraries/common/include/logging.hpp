#pragma once

#include <boost/log/trivial.hpp>

namespace common::logging {

#define LOG_DEBUG() BOOST_LOG_TRIVIAL(debug)
#define LOG_INFO() BOOST_LOG_TRIVIAL(info)
#define LOG_WARNING() BOOST_LOG_TRIVIAL(warning)
#define LOG_ERROR() BOOST_LOG_TRIVIAL(error)

#define LOG() BOOST_LOG_TRIVIAL(info)

void InitLog();

} // namespace common::logging