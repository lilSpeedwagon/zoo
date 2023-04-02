#include <binary.hpp>


namespace common::binary {

namespace {

void SetupStreamExceptions(
    std::basic_ios<BinaryByteT, std::char_traits<BinaryByteT>>& stream) {
    stream.exceptions(
        std::ifstream::failbit | std::ifstream::eofbit | std::ifstream::badbit);
}

} // namespace

EofException::EofException() : std::runtime_error("end of binary file is reached") {}

BinaryInStream::BinaryInStream(const std::filesystem::path& path)
    : stream_(path, std::ios::in) {
    SetupStreamExceptions(stream_);
}

BinaryInStream::BinaryInStream(StreamT&& stream) 
    : stream_(std::move(stream)) {
    SetupStreamExceptions(stream_);
}

BinaryInStream::BinaryInStream(BinaryInStream&& other) {
    std::swap(stream_, other.stream_);
}

BinaryInStream::~BinaryInStream() {}

BinaryInStream& BinaryInStream::operator=(BinaryInStream&& other) {
    std::swap(stream_, other.stream_);
    return *this;
}

BinaryInStream& BinaryInStream::operator>>(std::string& str) {
    size_t count{};
    *this >> count;
    str.resize(count);
    stream_.read(reinterpret_cast<BinaryByteT*>(str.data()), count);
    return *this;
}

BinaryInStream& BinaryInStream::operator>>(
    std::chrono::time_point<std::chrono::system_clock>& time_point) {
    int64_t count{};
    *this >> count;
    time_point = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::nanoseconds(count));
    return *this;
}

BinaryOutStream::BinaryOutStream(const std::filesystem::path& path)
    : stream_(path, std::ios::out) {
    SetupStreamExceptions(stream_);
}

BinaryOutStream::BinaryOutStream(StreamT&& stream) 
    : stream_(std::move(stream)) {
    SetupStreamExceptions(stream_);
}

BinaryOutStream::BinaryOutStream(BinaryOutStream&& other) {
    std::swap(stream_, other.stream_);
}

BinaryOutStream::~BinaryOutStream() {}

BinaryOutStream& BinaryOutStream::operator=(BinaryOutStream&& other) {
    std::swap(stream_, other.stream_);
    return *this;
}

BinaryOutStream& BinaryOutStream::operator<<(const std::string& str) {
    *this << str.size();
    stream_.write(reinterpret_cast<const BinaryByteT*>(str.data()), str.size());
    return *this;
}

BinaryOutStream& BinaryOutStream::operator<<(
    const std::chrono::time_point<std::chrono::system_clock>& time_point) {
    const auto count = std::chrono::duration_cast<std::chrono::nanoseconds>(
        time_point.time_since_epoch()).count();
    *this << count;
    return *this;
}

} // namespace common::binary
