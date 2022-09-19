#include <binary.hpp>

namespace common::binary {

BinaryInStream::BinaryInStream(std::ifstream&& stream) 
    : stream_(std::move(stream)) {}

BinaryInStream& BinaryInStream::operator>>(std::string& str) {
    size_t count{};
    *this >> count;
    str.resize(count);
    stream_.read(str.data(), count);
    return *this;
}

BinaryInStream& BinaryInStream::operator>>(
    std::chrono::time_point<std::chrono::system_clock>& time_point) {
    int64_t count{};
    *this >> count;
    time_point = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::seconds(count));
    return *this;
}

BinaryOutStream::BinaryOutStream(std::ofstream&& stream) 
    : stream_(std::move(stream)) {}

BinaryOutStream& BinaryOutStream::operator<<(const std::string& str) {
    *this << str.size();
    stream_.write(str.data(), str.size());
    return *this;
}

BinaryOutStream& BinaryOutStream::operator<<(
    const std::chrono::time_point<std::chrono::system_clock>& time_point) {
    const auto count = time_point.time_since_epoch().count();
    *this << count;
    return *this;
}

} // namespace common::binary
