#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>

#include <common/include/strong_typedef.hpp>

#include <models/namespace.hpp>

namespace documents::models {

using DocumentId = 
    common::types::StrongTypedef<uint64_t, struct DocumentIdTag>;
using DocumentPayload = 
    common::types::StrongTypedef<std::string, struct DocumentPayloadTag>;
using DocumentPayloadPtr = std::shared_ptr<DocumentPayload>;

struct Document {
    DocumentId id{};
    std::chrono::time_point<std::chrono::system_clock> created{};
    std::chrono::time_point<std::chrono::system_clock> updated{};
    std::string name{};
    std::string owner{};
    std::optional<std::string> namespace_name{};
    DocumentPayloadPtr payload_ptr{};
};

struct DocumentInput {
    std::string name{};
    std::string owner{};
    std::string namespace_name{};
    DocumentPayload payload{};
};

struct DocumentUpdateInput {
    std::string name{};
    std::string namespace_name{};
    std::optional<DocumentPayload> payload{};
};

using DocumentPtr = std::shared_ptr<Document>;

} // namespace documents::models
