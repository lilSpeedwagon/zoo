#include "sink_string.hpp"

#include <iostream>

namespace common::logging {

void SinkString::Write(const std::stringstream& buffer) {
    buffer_ << buffer.str();
}

std::string SinkString::GetString() const {
    return buffer_.str();
}

void SinkString::Reset() {
    buffer_.clear();
}

} // namespace common::logging