#include "sink_stdout.hpp"

#include <iostream>

namespace common::logging {

void SinkStdout::Write(const std::stringstream& buffer) {
    std::cout << buffer.str() << std::flush;
}

} // namespace common::logging