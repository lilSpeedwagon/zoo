#include <catch2/catch.hpp>

#include <components/include/component_base.hpp>
#include <components/include/components_engine.hpp>

namespace components::tests {

namespace {

class ComponentsEngineFixture {
public:
    ComponentsEngineFixture() {
        auto& engine = components::ComponentsEngine::GetInstance();
        engine.Clear();
    }
};

class CustomComponent 
    : public components::ComponentBase {
public:
    static constexpr const char* kName = "custom";

    CustomComponent() {}
    virtual ~CustomComponent() {}

    void Init() override {
        is_ready_ = true;
    }
    void Reset() override {}
    const char* Name() const override {
        return kName;
    }
};

} // namespace

TEST_CASE_METHOD(ComponentsEngineFixture, "Register component twice", "[Components engine]") {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    engine.Register(component_ptr);
    CHECK_THROWS_WITH(engine.Register(component_ptr),
                      Catch::Matchers::Contains("component 'custom' already registered"));
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Get component", "[Components engine]") {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    engine.Register(component_ptr);
    auto result = engine.Get<CustomComponent>();
    CHECK(!result->IsReady());
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Get not registered", "[Components engine]") {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    CHECK_THROWS_WITH(engine.Get<CustomComponent>(),
                      Catch::Matchers::Contains("component 'custom' is not registered"));
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Get by name", "[Components engine]") {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    engine.Register(component_ptr);
    auto result = engine.Get("custom");
    CHECK(result != nullptr);
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Get by name not registered", "[Components engine]") {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto result = engine.Get("custom");
    CHECK(result == nullptr);
}

TEST_CASE_METHOD(ComponentsEngineFixture, "Init components", "[Components engine]") {
    auto& engine = components::ComponentsEngine::GetInstance();
    auto component_ptr = std::make_shared<CustomComponent>();
    engine.Register(component_ptr);
    engine.Init();
    auto result = engine.Get<CustomComponent>();
    CHECK(result->IsReady());
}

} // namespace components::tests