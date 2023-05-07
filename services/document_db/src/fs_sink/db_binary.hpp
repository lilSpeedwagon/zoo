#pragma once

#include <optional>

#include <common/include/binary.hpp>

#include <models/document.hpp>


namespace documents::fs_sink {

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const std::optional<models::DocumentPosition>& position);

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const models::DocumentInfo& document_info);

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const models::Document& document);

common::binary::BinaryInStream& operator>>(common::binary::BinaryInStream& stream,
                                           std::optional<models::DocumentPosition>& position);

common::binary::BinaryInStream& operator>>(common::binary::BinaryInStream& stream,
                                           models::DocumentInfo& document_info);

common::binary::BinaryInStream& operator>>(common::binary::BinaryInStream& stream,
                                           models::Document& document);

} // namespace documents::fs_sink
