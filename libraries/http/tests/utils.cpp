#include <string>
#include <unordered_map>

#include <catch2/catch.hpp>

#include <http/include/models.hpp>
#include <http/include/utils.hpp>

namespace http::tests::utils {

namespace {

struct TestCase {
    std::string path{};
    std::unordered_map<std::string, std::string> expected{};
};

} // namespace

TEST_CASE("GetPath", "[Utils]") {
    auto test_case = GENERATE(
        TestCase{"/path", {}},
        TestCase{"/path?", {}},
        TestCase{"/path?key", {}},
        TestCase{"/path?key=", {{"key", ""}}},
        TestCase{"/path?key=value", {{"key", "value"}}},
        TestCase{"/path?key=value&", {{"key", "value"}}},
        TestCase{"/path?key=value&key2", {{"key", "value"}}},
        TestCase{"/path?key=value&key2=", {{"key", "value"}, {"key2", ""}}},
        TestCase{"/path?key=value&key2=value2", {{"key", "value"}, {"key2", "value2"}}},
        TestCase{"/path?key=value&key=value2", {{"key", "value2"}}},
        TestCase{"/path?key=value&&key2=value2", {{"key", "value"}, {"&key2", "value2"}}},
        TestCase{"/path??key=value", {{"?key", "value"}}},
        TestCase{"/path?=value", {}},
        TestCase{"/?key=value", {{"key", "value"}}},
        TestCase{"?key=value", {{"key", "value"}}},
        TestCase{"?key=value/path", {{"key", "value/path"}}}
    );

    http::Request request{};
    request.target(test_case.path);
    CHECK(http::utils::GetParams(request) == test_case.expected);
}


} // namespace http::tests::utils