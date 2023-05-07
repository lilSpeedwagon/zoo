#include "db_binary.hpp"


namespace documents::fs_sink {

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const std::optional<models::DocumentPosition>& position) {
    stream << position.has_value();
    if (position.has_value()) {
        stream << position.value().page_index;
        stream << position.value().page_offset;
    }
    return stream;
}

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const models::DocumentInfo& document_info) {
    stream << document_info.id.GetUnderlying();
    stream << document_info.created;
    stream << document_info.updated;
    stream << document_info.name;
    stream << document_info.namespace_name;
    stream << document_info.owner;
    stream << document_info.position;
    return stream;
}

common::binary::BinaryOutStream& operator<<(common::binary::BinaryOutStream& stream,
                                            const models::Document& document) {
    stream << document.info;
    stream << document.payload;
    return stream;
}

common::binary::BinaryInStream& operator>>(common::binary::BinaryInStream& stream,
                                           std::optional<models::DocumentPosition>& position) {
    bool has_value = false;
    stream >> has_value;
    
    if (has_value) {
        models::DocumentPosition temp{};
        stream >> temp.page_index;
        stream >> temp.page_offset;
        position = std::make_optional(std::move(temp));
    } else {
        position = std::nullopt;
    }

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
    stream >> temp.position;
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
