#pragma once

#include <chrono>
#include <fstream>
#include <istream>
#include <ostream>
#include <vector>

#include <boost/noncopyable.hpp>

namespace common::binary {

using BinaryByteT = char;

/// @class Raised when trying to read binary file after EOF.
class EofException : public std::runtime_error {
public:
    EofException() : std::runtime_error("end of binary file is reached") {}
};

/// @class std::ifstream wrapper for binary I/O
class BinaryInStream final : private boost::noncopyable {
public:
    using StreamT = std::basic_ifstream<BinaryByteT, std::char_traits<BinaryByteT>>;

    BinaryInStream(const std::string& path);
    BinaryInStream(StreamT&& stream);
    ~BinaryInStream();

    template<typename T, 
             typename std::enable_if<std::is_arithmetic_v<T>, bool>::type = true>
    BinaryInStream& operator>>(T& value) {
        constexpr size_t buffer_size = sizeof(value);
        stream_.read(reinterpret_cast<BinaryByteT*>(&value), buffer_size);
        return *this;
    }

    template<typename T>
    BinaryInStream& operator>>(std::vector<T>& data) {
        size_t count{};
        *this >> count;
        data.resize(count);
        for (size_t i = 0; i < count; i++) {
            T item{};
            *this >> item;
            data[i] = std::move(item);
        } 
        return *this;
    }
    
    BinaryInStream& operator>>(std::string& str);
    BinaryInStream& operator>>(
        std::chrono::time_point<std::chrono::system_clock>& time_point);

private:
    StreamT stream_;
};

/// @class std::ofstream wrapper for binary I/O
class BinaryOutStream final : private boost::noncopyable {
public:
    using StreamT = std::basic_ofstream<BinaryByteT, std::char_traits<BinaryByteT>>;

    BinaryOutStream(const std::string& path);
    BinaryOutStream(StreamT&& stream);
    ~BinaryOutStream();

    template<typename T, 
             typename std::enable_if<std::is_arithmetic_v<T>, bool>::type = true>
    BinaryOutStream& operator<<(const T& value) {
        constexpr size_t buffer_size = sizeof(value);
        stream_.write(reinterpret_cast<const BinaryByteT*>(&value), buffer_size);
        return *this;
    }

    template<typename T>
    BinaryOutStream& operator<<(const std::vector<T>& data) {
        *this << data.size();
        for (const auto& item : data) {
            *this << item;
        }
        return *this;
    }
    
    BinaryOutStream& operator<<(const std::string& str);
    BinaryOutStream& operator<<(
        const std::chrono::time_point<std::chrono::system_clock>& time_point);

private:
    StreamT stream_;
};

} // namespace common::binary
