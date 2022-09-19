#pragma once

#include <chrono>
#include <fstream>
#include <istream>
#include <ostream>
#include <vector>

namespace common::binary {

/// @class std::ifstream wrapper for binary I/O
class BinaryInStream final {
public:
    BinaryInStream(std::ifstream&& stream);

    template<typename T>
    BinaryInStream& operator>>(T& value) {
        if constexpr (std::is_arithmetic_v<T>) {
            stream_.read(reinterpret_cast<char*>(&value), sizeof(value));
        } else {
            stream_ >> value;
        }
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
    std::ifstream stream_;
};

/// @class std::ostream wrapper for binary I/O
class BinaryOutStream final {
public:
    BinaryOutStream(std::ofstream&& stream);

    template<typename T>
    BinaryOutStream& operator<<(const T& value) {
        if constexpr (std::is_arithmetic_v<T>) {
            stream_.write(reinterpret_cast<const char*>(&value), sizeof(value));
        } else {
            stream_ << value;
        }
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
    std::ofstream stream_;
};

} // namespace common::binary
