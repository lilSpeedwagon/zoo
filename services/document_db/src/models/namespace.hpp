#pragma once

#include <memory>
#include <string>

#include <common/include/strong_typedef.hpp>

namespace documents::models {

using NamespaceId = common::types::StrongTypedef<uint64_t, struct NamespaceIdTag>;

struct Namespace {
    NamespaceId id{};
    std::string name{};
};

using NamespacePtr = std::shared_ptr<Namespace>;

} // namespace documents::models
