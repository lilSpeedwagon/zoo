#pragma once

#include <exception>

#include <http/include/models.hpp>

namespace http::exceptions {

/// @class Base class for HTTP exceptions.
class HttpError : public std::runtime_error {
public:
    HttpError(const char* msg) : std::runtime_error(msg) {}
    virtual ~HttpError() {}
    virtual http::Status Code() const;
};

/// @class Bad request error. Code 400.
class BadRequest : public HttpError {
public:
    BadRequest(const char* msg) : HttpError(msg) {}
    virtual ~BadRequest() {}
    http::Status Code() const override {
        return http::Status::bad_request;
    }
};

/// @class Not found error. Code 404.
class NotFound : public HttpError {
public:
    NotFound(const char* msg) : HttpError(msg) {}
    virtual ~NotFound() {}
    http::Status Code() const override {
        return http::Status::not_found;
    }
};

/// @class Server error. Code 500.
class ServerError : public HttpError {
public:
    ServerError(const char* msg) : HttpError(msg) {}
    virtual ~ServerError() {}
    http::Status Code() const override {
        return http::Status::internal_server_error;
    }
};

} // namespace http::exceptions