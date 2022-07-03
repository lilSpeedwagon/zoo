#pragma once

#include <common/src/logging/sink_base.hpp>

namespace common::logging {

class SinkFS : public LoggerSinkBase {
public:
    SinkFS(const std::string& path, const std::string& file_prefix);
    virtual ~SinkFS();
    virtual void Write(const std::stringstream& buffer) override;

private:
    std::string path_;
    std::string file_prefix_;
    std::string full_path_;
};

} // namespace common::logging