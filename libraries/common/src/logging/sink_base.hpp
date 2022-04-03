#pragma once

#include <sstream>

namespace common::logging {

/// @class logger sink base class (fs, cout, etc.)
class LoggerSinkBase {
public:
    virtual ~LoggerSinkBase() = default;
    virtual void Write(const std::stringstream& buffer) = 0;
};

using LoggerSinkPtr = std::shared_ptr<LoggerSinkBase>;

} // namespace common::logging