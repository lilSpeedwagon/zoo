#include <chrono>
#include <string>

#include <catch2/catch.hpp>

#include <common/include/format.hpp>

void CheckString(const std::string& str, const char* expected) {
    CHECK(str == std::string(expected));
}


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

    SECTION("String") {
        CheckString(ToString(std::chrono::system_clock::time_point()),
                    "Thu Jan  1 03:00:00 1970");
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
        CheckString(Format("{}", std::chrono::system_clock::time_point()),
                    "Thu Jan  1 03:00:00 1970");
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