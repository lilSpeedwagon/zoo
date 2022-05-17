#include <optional>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include <common/include/config.hpp>

namespace {

struct NestedConfig {
    std::string nested_value{};
};

struct ConfigStruct {
    std::string str_value{};
    int int_value{};
    bool bool_value{};
    NestedConfig obj_value{};
    std::vector<int> arr_value{};
    std::optional<std::string> opt_value{};
};

bool operator==(const ConfigStruct& lhs, const ConfigStruct& rhs) {
    return lhs.str_value == rhs.str_value &&
        lhs.int_value == rhs.int_value &&
        lhs.bool_value == rhs.bool_value &&
        lhs.obj_value.nested_value == rhs.obj_value.nested_value &&
        lhs.arr_value == rhs.arr_value &&
        lhs.opt_value == rhs.opt_value;
}

void from_json(const common::json::json& data, ConfigStruct& config) {
    data.at("str_key").get_to(config.str_value);
    data.at("int_key").get_to(config.int_value);
    data.at("bool_key").get_to(config.bool_value);
    data.at("obj_key").at("nested_key").get_to(config.obj_value.nested_value);
    data.at("arr_key").get_to(config.arr_value);
    if (data.contains("opt_key")) {
        data.at("opt_key").get_to(config.opt_value);
    }
}

} // namespace

TEST_CASE("Get", "[Config]") {
    common::json::json config_data = {
        {"str_key", "value"},
        {"int_key", 10},
        {"bool_key", true},
        {"obj_key", {{"nested_key", "nested_value"}}},
        {"arr_key", {1, 2, 3}},
    };

    auto config = common::config::Config::FromJson(config_data);
    auto config_struct = config.Get<ConfigStruct>();

    ConfigStruct epxected_struct = {
        "value",            // str_value
        10,                 // int_value
        true,               // bool_value
        {"nested_value"},   // obj_value
        {1, 2, 3},          // arr_value
        std::nullopt,       // opt_value
    };
    CHECK(config_struct == epxected_struct);
}

TEST_CASE("Get invalid", "[Config]") {
    common::json::json config_data = {
        {"invalid_key", "value"},
    };

    auto config = common::config::Config::FromJson(config_data);
    CHECK_THROWS_WITH(config.Get<ConfigStruct>(), 
                      Catch::Matchers::Contains("key 'str_key' not found"));
}

TEST_CASE("GetPath", "[Config]") {
    common::json::json config_data = {
        {"str_key", "value"},
        {"int_key", 10},
        {"bool_key", true},
        {"obj_key", {{"nested_key", "nested_value"}}},
        {"arr_key", {1, 2, 3}},
    };

    auto config = common::config::Config::FromJson(config_data);
    
    SECTION("One step path") {
        auto config_opt = config.GetPath<std::string>("str_key");
        CHECK(config_opt.has_value());
        CHECK(config_opt.value() == std::string("value"));
    }

    SECTION("Two steps path") {
        auto config_opt = config.GetPath<std::string>("obj_key/nested_key");
        CHECK(config_opt.has_value());
        CHECK(config_opt.value() == std::string("nested_value"));
    }

    SECTION("Custom delimiter") {
        auto config_opt = config.GetPath<std::string>("obj_key:nested_key", ':');
        CHECK(config_opt.has_value());
        CHECK(config_opt.value() == std::string("nested_value"));
    }

    SECTION("Empty path") {
        auto config_opt = config.GetPath<ConfigStruct>("");
        CHECK(config_opt.has_value());
        ConfigStruct epxected_struct = {
            "value",            // str_value
            10,                 // int_value
            true,               // bool_value
            {"nested_value"},   // obj_value
            {1, 2, 3},          // arr_value
            std::nullopt,       // opt_value
        };
        CHECK(config_opt.value() == epxected_struct);
    }

    SECTION("Path not found") {
        auto config_opt = config.GetPath<std::string>("unknown_path");
        CHECK(!config_opt.has_value());
    }

    SECTION("Invalid path") {
        CHECK_THROWS_WITH(config.GetPath<std::string>("key//key2"), 
                          Catch::Matchers::Contains("invalid config path"));
    }
}
