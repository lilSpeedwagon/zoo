#include <chrono>
#include <string>

#include <catch2/catch.hpp>

#include <common/include/format.hpp>
#include <common/include/strong_typedef.hpp>

namespace common::tests::format {

namespace {

std::chrono::system_clock::time_point GetDefaultTime() {
    std::tm tm{0, 0, 0, 1, 0, 70, 0, 0, -1};
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

void CheckString(const std::string& str, const char* expected) {
    CHECK(str == std::string(expected));
}

} // namespace


TEST_CASE("ToString", "[Format]") {
    using common::format::ToString;

    SECTION("Numbers") {
        CheckString(ToString(1), "1");
        CheckString(ToString(1.0), "1.000000");
        CheckString(ToString(-1.0), "-1.000000");
    }

    SECTION("Boolean") {
        CheckString(ToString(false), "false");
        CheckString(ToString(true), "true");
    }

    SECTION("String") {
        CheckString(ToString("hello"), "hello");
        CheckString(ToString(std::string("hello")), "hello");
    }

    SECTION("Timepoint") {
        CheckString(ToString(GetDefaultTime()),
                    "Thu Jan  1 00:00:00 1970");
    }

    SECTION("StrongTypedef") {
        using Custom = types::StrongTypedef<std::string, struct CustomTag>;
        auto str = Custom(std::string("hello"));
        CheckString(ToString(str), "hello");
    }
}


TEST_CASE("Format", "[Format]") {
    using common::format::Format;

    SECTION("Without substitution") {
        CheckString(Format(""), "");
        CheckString(Format("Hello world!"), "Hello world!");
        CheckString(Format("{ }"), "{ }");
    }

    SECTION("With substitution (int)") {
        CheckString(Format("{}", 1), "1");
        CheckString(Format("Result: {}", 100), "Result: 100");
        CheckString(Format("{}-{}-{}", 1, 2, 3), "1-2-3");
        CheckString(Format("num {}", -1), "num -1");
    }

    SECTION("Substitution (float)") {
        CheckString(Format("{}", 1.0f), "1.000000");
        CheckString(Format("Result: {}", 3.25f), "Result: 3.250000");
    }

    SECTION("Substitution (bool)") {
        CheckString(Format("{}", false), "false");
        CheckString(Format("{}", true), "true");
    }

    SECTION("Substitution (string)") {
        CheckString(Format("name: {}", "bob"), "name: bob");
        CheckString(Format("{}{}{}", "hello", " ", "world"), "hello world");
        CheckString(Format("{}{}{}", 'a', 'b', 'c'), "979899");
    }

    SECTION("Substitution (time)") {
        CheckString(Format("{}", GetDefaultTime()),
                    "Thu Jan  1 00:00:00 1970");
    }

    SECTION("Substitution (strong typedef)") {
        using Custom = types::StrongTypedef<std::string, struct CustomTag>;
        CheckString(Format("{}", Custom(std::string("hello"))), "hello");
    }

    SECTION("Substitution (mixed)") {
        CheckString(Format("int: {}, string: {}, bool: {}", 10, "str", false),
                    "int: 10, string: str, bool: false");
    }
    
    SECTION("Bad format") {
        CHECK_THROWS_WITH(Format("", 1), 
                          Catch::Matchers::Contains("extra arguments in Format() call"));
        CHECK_THROWS_WITH(Format("{} {}", 1),
                          Catch::Matchers::Contains("not enough arguments in Format() call"));
    }
}


TEST_CASE("Argument with placeholder", "[Format]") {
    SECTION("Placeholder as argument") {
        auto result = common::format::Format("result: {}", "{}");
        CheckString(result, "result: {}");
    }

    SECTION("Collapsed placeholder") {
        auto result = common::format::Format("result: {{}}", "");
        CheckString(result, "result: {}");
    }

    SECTION("Appended placeholder") {
        auto result = common::format::Format("result: {}}", "{");
        CheckString(result, "result: {}");
    }
}


TEST_CASE("Shrink", "[Format]") {
    auto result = common::format::ShrinkString("Lorem ipsum", 5);
    CheckString(result, "Lorem... (6 more chars)");
}

} // namespace common::tests::format