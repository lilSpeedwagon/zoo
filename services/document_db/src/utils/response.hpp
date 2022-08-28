#pragma once

#include <vector>

#include <http/include/models.hpp>

#include <models/document.hpp>

namespace documents::utils::response {

http::Response ToResponse(models::Document&& document);

http::Response ToResponse(std::vector<models::Document>&& documents);

} // namespace documents::utils::response
