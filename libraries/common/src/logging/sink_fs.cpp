#include "sink_fs.hpp"

#include <chrono>
#include <fstream>
#include <sstream>

#include <common/include/format.hpp>
#include <common/include/utils/errors.hpp>

namespace common::logging {

namespace {

std::string GetFullPath(const std::string& path,
                        const std::string& file_prefix) {
    static constexpr std::string_view kTimeFormat = "%F_%Hh%Mm%S";
    std::stringstream ss{};
    ss << path.c_str();
    if (path.back() != '\\' && path.back() != '/') {
        ss << '\\';
    }
    ss << file_prefix;
    const auto now = std::chrono::system_clock::now();
    ss << format::TimePointToString(now, kTimeFormat);
    ss << ".log";
    return ss.str();
}

} // namespace

SinkFS::SinkFS(const std::string& path, const std::string& file_prefix) 
    : path_(path), file_prefix_(file_prefix),
      full_path_(GetFullPath(path, file_prefix)) {}

SinkFS::~SinkFS() {}

void SinkFS::Write(const std::stringstream& buffer) {
    std::ofstream stream(full_path_, std::ios::app);
    if (stream.is_open() && stream.good()) {
        stream << buffer.str();
    } else {
        throw std::runtime_error(format::Format(
            "Cannot open log file: {}", utils::errors::GetLastError()));
    }
}

} // namespace common::logging
