#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <list>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>


namespace common::binary {

using BinaryByteT = char;

/// @class Raised when trying to read binary file after EOF.
class EofException : public std::runtime_error {
public:
    EofException();
};

/// @class std::ifstream wrapper for binary I/O. Intended to read data stored via BinaryOutStream.
/// Binary stream wrappers provide formatted streams interface, but store data more compactly.
/// Main features are the following:
/// - storing numbers in binary format instead of formatted characters
/// - storing of dynamic containers (std::string, std::vector, etc.)
class BinaryInStream final : private boost::noncopyable {
public:
    using StreamT = std::basic_ifstream<BinaryByteT, std::char_traits<BinaryByteT>>;

    /// @brief ctor
    /// @param path wrappable file path 
    BinaryInStream(const std::filesystem::path& path);

    /// @brief ctor
    /// @param stream already opened file to wrap
    BinaryInStream(StreamT&& stream);

    /// @brief Move ctor
    /// @param other other stream instance
    BinaryInStream(BinaryInStream&& other);

    /// @brief dtor
    ~BinaryInStream();

    /// @brief Move assignment operator
    /// @param other other stream
    /// @return ref to self
    BinaryInStream& operator=(BinaryInStream&& other);

    /// @brief Reads a single value of an arithmetic type T from the file.
    /// @tparam T value type
    /// @param value ref to value destination
    /// @return ref to self
    template<typename T, 
             typename std::enable_if<std::is_arithmetic_v<T>, bool>::type = true>
    BinaryInStream& operator>>(T& value) {
        constexpr size_t buffer_size = sizeof(value);
        stream_.read(reinterpret_cast<BinaryByteT*>(&value), buffer_size);
        return *this;
    }

    /// @brief Reads a dynamic container of type T.
    /// @tparam T container type
    /// @param data ref to destination container
    /// @return ref to self
    template<typename T,
             typename std::enable_if<std::is_same<T, std::vector<typename T::value_type>>::value ||
                                     std::is_same<T, std::list<typename T::value_type>>::value, bool>::type = true>
    BinaryInStream& operator>>(T& data) {
        using value_type = typename T::value_type;

        size_t count{};
        *this >> count;
        if constexpr (std::is_same<T, std::vector<value_type>>::value) {
            data.reserve(count);
        }

        for (size_t i = 0; i < count; i++) {
            typename T::value_type item{};
            *this >> item;
            data.emplace_back(std::move(item));
        }
        return *this;
    }
    
    /// @brief Reads a single formatted string.
    /// @param str ref to value destination
    /// @return ref to self
    BinaryInStream& operator>>(std::string& str);
    
    /// @brief Reads a single chrono::time_point value.
    /// @param time_point ref to value destination
    /// @return ref to result
    BinaryInStream& operator>>(
        std::chrono::time_point<std::chrono::system_clock>& time_point);

private:
    StreamT stream_;
};

/// @class std::ofstream wrapper for binary I/O.
/// Binary stream wrappers provide formatted streams interface, but store data more compactly.
/// Main features are the following:
/// - storing numbers in binary format instead of formatted characters
/// - storing of dynamic containers (std::string, std::vector, etc.)
class BinaryOutStream final : private boost::noncopyable {
public:
    using StreamT = std::basic_ofstream<BinaryByteT, std::char_traits<BinaryByteT>>;

    /// @brief ctor
    /// @param path wrappable file path 
    BinaryOutStream(const std::filesystem::path& path);

    /// @brief ctor
    /// @param stream already opened file to wrap
    BinaryOutStream(StreamT&& stream);

    /// @brief Move ctor
    /// @param other other stream instance
    BinaryOutStream(BinaryOutStream&& other);

    /// @brief dtor
    ~BinaryOutStream();

    /// @brief Move assignment operator
    /// @param other other stream
    /// @return ref to self
    BinaryOutStream& operator=(BinaryOutStream&& other);

    /// @brief Writes a single value of an arithmetic type T to the file.
    /// @tparam T value type
    /// @param value value to store
    /// @return ref to self
    template<typename T, 
             typename std::enable_if<std::is_arithmetic_v<T>, bool>::type = true>
    BinaryOutStream& operator<<(const T& value) {
        constexpr size_t buffer_size = sizeof(value);
        stream_.write(reinterpret_cast<const BinaryByteT*>(&value), buffer_size);
        return *this;
    }

    /// @brief Writes a dynamic container of type T to the file.
    /// @tparam T container type
    /// @param data container to store
    /// @return ref to self
    template<typename T,
             typename std::enable_if<std::is_same<T, std::vector<typename T::value_type>>::value ||
                                     std::is_same<T, std::list<typename T::value_type>>::value, bool>::type = true>
    BinaryOutStream& operator<<(const T& data) {
        *this << data.size();
        for (const auto& item : data) {
            *this << item;
        }
        return *this;
    }
    
    /// @brief Writes a single formatted string.
    /// @param str string to store
    /// @return ref to self
    BinaryOutStream& operator<<(const std::string& str);

    /// @brief Writes a single chrono::time_point value.
    /// @param time_point time point to store
    /// @return ref to result
    BinaryOutStream& operator<<(
        const std::chrono::time_point<std::chrono::system_clock>& time_point);

private:
    StreamT stream_;
};

} // namespace common::binary
