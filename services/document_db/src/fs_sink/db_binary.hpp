#pragma once

#include <common/include/binary.hpp>

#include <models/document.hpp>


namespace documents::fs_sink {

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const models::DocumentInfo& document_info) {
    stream << document_info.id.GetUnderlying();
    stream << document_info.created;
    stream << document_info.updated;
    stream << document_info.name;
    stream << document_info.namespace_name;
    stream << document_info.owner;
    return stream;
}

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const models::Document& document) {
    stream << document.info;
    stream << document.payload;
    return stream;
}

common::binary::BinaryInStream& operator>>(common::binary::BinaryInStream& stream,
                                           models::DocumentInfo& document_info) {
    models::DocumentInfo temp{};
    stream >> temp.id;
    stream >> temp.created;
    stream >> temp.updated;
    stream >> temp.name;
    stream >> temp.namespace_name;
    stream >> temp.owner;
    document_info = std::move(temp);
    return stream;
}

common::binary::BinaryInStream& operator>>(common::binary::BinaryInStream& stream,
                                           models::Document& document) {
    models::Document temp{};
    stream >> temp.info;
    stream >> temp.payload;
    document = std::move(temp);
    return stream;
}

} // namespace documents::fs_sink
