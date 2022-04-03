#pragma once

#include <sstream>

#include <common/src/logging/sink_base.hpp>

namespace common::logging {

/// @class Sink intended for the log testing.
/// With the sink it is possible to check written log in isolated environment.
class SinkString : public LoggerSinkBase {
public: 
    SinkString() = default;
    virtual ~SinkString() = default;

    virtual void Write(const std::stringstream& buffer) override;
    std::string GetString() const;
    void Reset();

private:
    std::stringstream buffer_;
};

} // namespace common::logging