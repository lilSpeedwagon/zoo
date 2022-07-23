#include "dict.hpp"

#include <common/include/utils/algo.hpp>

namespace dummy::components {

DummyDict::DummyDict() : dict_{} {}

DummyDict::~DummyDict() {}

const char* DummyDict::Name() const {
    return kName;
}

void DummyDict::Init() {}

void DummyDict::Reset() {
    dict_.clear();
}

void DummyDict::Add(const std::string& key, const std::string& value) {
    dict_[key] = value;
}

std::optional<std::string> DummyDict::Get(const std::string& key) const {
    return common::utils::algo::GetOptional(dict_, key);
}

} // namespace dummy::components