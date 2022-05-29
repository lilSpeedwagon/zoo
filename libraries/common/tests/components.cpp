#include <catch2/catch.hpp>

#include <common/include/component_base.hpp>
#include <common/include/components_engine.hpp>

namespace common::tests::components {

namespace {

class ComponentsEngineFixture {
public:
    ComponentsEngineFixture() {
        auto& engine = common::components::ComponentsEngine::GetInstance();
        engine.Reset();
    }
};

class CustomComponent 
    : public common::components::ComponentBase {
public:
    static constexpr const char* kName = "custom";

    CustomComponent() {}
    virtual ~CustomComponent() {}

    void Init() override {
        is_ready_ = true;
    }
    const char* Name() const override {
        return kName;
    }
};

} // namespace

TEST_CASE_METHOD(ComponentsEngineFixture, "Get component", "[Components engine]") {
    auto& engine = common::components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    engine.Register(component_ptr);
    auto result = engine.Get<CustomComponent>();
    CHECK(!result->IsReady());
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Get not registered", "[Components engine]") {
    auto& engine = common::components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    CHECK_THROWS_WITH(engine.Get<CustomComponent>(),
                      Catch::Matchers::Contains("component 'custom' is not registered"));
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Init components", "[Components engine]") {
    auto& engine = common::components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    engine.Register(component_ptr);
    engine.Init();
    auto result = engine.Get<CustomComponent>();
    CHECK(result->IsReady());
}

} // namespace common::tests::components