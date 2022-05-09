#pragma once

#include <http/include/models.hpp>


namespace http::utils {

std::string ToString(const Method method) {
    switch(method) {
        case Method::get:
            return "GET";
    }
}

} // namespace http::utils