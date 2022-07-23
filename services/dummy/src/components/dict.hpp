#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <components/include/component_base.hpp>


namespace dummy::components {

class DummyDict final
    : public ::components::ComponentBase {
public:
    static constexpr const char* kName = "dummy-dict";

    DummyDict();
    virtual ~DummyDict();

    const char* Name() const override;

    void Init() override;
    void Reset() override;

    void Add(const std::string& key, const std::string& value);
    std::optional<std::string> Get(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> dict_;
};

} // namespace dummy::components