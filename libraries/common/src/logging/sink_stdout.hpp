#pragma once

#include <common/src/logging/sink_base.hpp>

namespace common::logging {

class SinkStdout : public LoggerSinkBase {
public: 
    SinkStdout() = default;
    virtual ~SinkStdout() = default;

    virtual void Write(const std::stringstream& buffer) override;
};

} // namespace common::logging