#include <binary.hpp>

#include <common/include/format.hpp>


namespace common::binary {

namespace {

static const std::ios_base::openmode kFileWriteMode = std::ios::binary | std::ios::out | std::ios::in;
static const std::ios_base::openmode kFileReadMode = std::ios::binary | std::ios::in;

void SetupStreamExceptions(
    std::basic_ios<BinaryByteT, std::char_traits<BinaryByteT>>& stream) {
    stream.exceptions(
        std::ifstream::failbit | std::ifstream::eofbit | std::ifstream::badbit);
}

void EnsureExist(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        // "touch" file to create it
        std::ofstream(path, std::ios::out);
    }
}

} // namespace

EofException::EofException() : std::runtime_error("end of binary file is reached") {}

BinaryInStream::BinaryInStream(const std::filesystem::path& path)
    : stream_(path, kFileReadMode) {
    Init();
}

BinaryInStream::BinaryInStream(StreamT&& stream) 
    : stream_(std::move(stream)) {
    Init();
}

BinaryInStream::BinaryInStream(BinaryInStream&& other) {
    std::swap(stream_, other.stream_);
}

BinaryInStream::~BinaryInStream() {}

bool BinaryInStream::Eof() const {
    return stream_.eof();
}

void BinaryInStream::Seek(size_t position) {
    stream_.seekg(position);
}

void BinaryInStream::Seek(BinaryStreamPosition position) {
    switch (position) {
        case BinaryStreamPosition::BEGIN:
            stream_.seekg(0, std::ios_base::beg);
            break;
        case BinaryStreamPosition::END:
            stream_.seekg(0, std::ios_base::end);
            break;
        default:
            throw std::logic_error(
                common::format::Format("Unknown BinaryStreamPosition: {}", static_cast<int>(position)));
    }
}

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

void BinaryInStream::Init() {
    SetupStreamExceptions(stream_);
    Seek(BinaryStreamPosition::BEGIN);
}

BinaryOutStream::BinaryOutStream(const std::filesystem::path& path) {
    EnsureExist(path);
    stream_ = StreamT(path, kFileWriteMode);
    Init();
}

BinaryOutStream::BinaryOutStream(StreamT&& stream) 
    : stream_(std::move(stream)) {
    Init();
}

BinaryOutStream::BinaryOutStream(BinaryOutStream&& other) {
    std::swap(stream_, other.stream_);
}

BinaryOutStream::~BinaryOutStream() {}

bool BinaryOutStream::Eof() const {
    return stream_.eof();
}

void BinaryOutStream::Seek(size_t position) {
    stream_.seekp(position);
}

void BinaryOutStream::Seek(BinaryStreamPosition position) {
    switch (position) {
        case BinaryStreamPosition::BEGIN:
            stream_.seekp(0, std::ios_base::beg);
            break;
        case BinaryStreamPosition::END:
            stream_.seekp(0, std::ios_base::end);
            break;
        default:
            throw std::logic_error(
                common::format::Format("Unknown BinaryStreamPosition: {}", static_cast<int>(position)));
    }
}

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

void BinaryOutStream::Init() {
    SetupStreamExceptions(stream_);
    Seek(BinaryStreamPosition::END);
}

} // namespace common::binary
