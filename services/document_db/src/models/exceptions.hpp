#pragma once

#include <exception>
#include <string>

#include <common/include/format.hpp>

#include <models/document.hpp>

namespace documents::exceptions {

class Exception : public std::runtime_error {
public:
    Exception(const char* msg) : std::runtime_error(msg) {}
    Exception(const std::string& msg) : std::runtime_error(msg.c_str()) {}
};

class NotFoundException : public Exception {
public:
    NotFoundException(models::DocumentId id) 
        : Exception(common::format::Format(
            "Document with id=\'{}\' not found", id).c_str()) {}
};

class InvalidStateException : public Exception {
public:
    InvalidStateException(const std::string& msg) : Exception(msg) {}
};

class BadRequestException : public Exception {
public:
    BadRequestException(const std::string& argument_name) 
        : Exception(common::format::Format(
            "Missing required argument \'{}\'", argument_name)) {}
};

} // namespace documents::exceptions
