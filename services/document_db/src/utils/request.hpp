#pragma once

#include <http/include/models.hpp>

#include <models/document.hpp>

namespace documents::utils::request {

/// @brief Gets id from request params.
models::DocumentId GetId(const http::Request& request);

} // namespace documents::utils::request
