#pragma once

#include <atomic>

#include <components/include/component_base.hpp>


namespace dummy::components {

class DummyCounter final
    : public ::components::ComponentBase {
public:
    static constexpr const char* kName = "dummy-counter";

    DummyCounter();
    virtual ~DummyCounter();

    const char* Name() const override;

    void Init() override;
    void Reset() override;

    size_t GetValue() const;
    size_t AddOne();

private:
    std::atomic<size_t> counter_;
};

} // namespace dummy::components