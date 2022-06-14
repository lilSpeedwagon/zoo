#include <string>
#include <unordered_map>

#include <catch2/catch.hpp>

#include <common/include/utils/algo.hpp>

namespace common::tests::utils {

TEST_CASE("ToLower", "[Algo]") {
    std::string str = "sOme StRinG-123";
    std::string expected = "some string-123";
    CHECK(common::utils::algo::ToLower(str) == expected);
}

TEST_CASE("ToUpper", "[Algo]") {
    std::string str = "sOme StRinG-123";
    std::string expected = "SOME STRING-123";
    CHECK(common::utils::algo::ToUpper(str) == expected);
}

TEST_CASE("GetOptional", "[Algo]") {
    std::unordered_map<std::string, int> map{};
    map["key"] = 1;
    map["key2"] = 2;

    CHECK(common::utils::algo::GetOptional(map, "key") == 1);
    CHECK(common::utils::algo::GetOptional(map, "key3") == std::nullopt);
}

TEST_CASE("GetOrDefault", "[Algo]") {
    std::unordered_map<std::string, int> map{};
    map["key"] = 1;
    map["key2"] = 2;

    CHECK(common::utils::algo::GetOrDefault(map, "key", 2) == 1);
    CHECK(common::utils::algo::GetOrDefault(map, "key3", 2) == 2);
}

} // namespace common::tests::utils