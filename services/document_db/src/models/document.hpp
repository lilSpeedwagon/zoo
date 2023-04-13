#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>

#include <common/include/json.hpp>
#include <common/include/strong_typedef.hpp>

#include <models/namespace.hpp>

namespace documents::models {

using DocumentId = 
    common::types::StrongTypedef<uint64_t, struct DocumentIdTag>;
using DocumentPayload = 
    common::types::StrongTypedef<std::string, struct DocumentPayloadTag>;
using DocumentPayloadPtr = std::shared_ptr<DocumentPayload>;

struct DocumentInput {
    std::string name{};
    std::string owner{};
    std::string namespace_name{};
    DocumentPayload payload{};
};

struct DocumentUpdateInput {
    std::optional<std::string> name{};
    std::optional<std::string> namespace_name{};
    std::optional<DocumentPayload> payload{};
};

struct DocumentInfo {
    DocumentId id{};
    std::chrono::time_point<std::chrono::system_clock> created{};
    std::chrono::time_point<std::chrono::system_clock> updated{};
    std::string name{};
    std::string owner{};
    std::string namespace_name{};
};
using DocumentInfoPtr = std::shared_ptr<DocumentInfo>;

struct Document {
    DocumentInfo info{};
    std::optional<DocumentPayload> payload{};
};

using DocumentInfoMap = 
    std::unordered_map<models::DocumentId, models::DocumentInfoPtr>;

void to_json(common::json::json& json, const Document& document);

} // namespace documents::models
