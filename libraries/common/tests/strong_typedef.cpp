#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <catch2/catch.hpp>

#include <common/include/strong_typedef.hpp>


namespace common::tests::types {

namespace {

struct Custom {
    int number;
    std::string string;
    bool flag;
};

bool operator==(const Custom& lhs, const Custom& rhs) {
    return lhs.number == rhs.number &&
        lhs.string == rhs.string &&
        lhs.flag == rhs.flag;
}

} // namespace

TEST_CASE("String", "[StrongTypedef]") {
    using Id = common::types::StrongTypedef<std::string, struct IdTag>;

    std::string id_str = "1234";
    Id id;
    id = id_str;

    Id id2(id_str);

    CHECK(id == id2);
    CHECK(id.GetUnderlying() == id_str);
    CHECK(id.GetUnderlying() == id2.GetUnderlying());
}

TEST_CASE("Integral", "[StrongTypedef]") {
    using Id = common::types::StrongTypedef<int, struct IdTag>;

    int id_int = 1234;
    Id id;
    id = id_int;

    Id id2(id_int);

    CHECK(id == id2);
    CHECK(id.GetUnderlying() == id_int);
    CHECK(id.GetUnderlying() == id2.GetUnderlying());
}

TEST_CASE("Boolean", "[StrongTypedef]") {
    using Id = common::types::StrongTypedef<bool, struct IdTag>;

    bool id_bool = true;
    Id id;
    id = id_bool;

    Id id2(id_bool);

    CHECK(id == id2);
    CHECK(id.GetUnderlying() == id_bool);
    CHECK(id.GetUnderlying() == id2.GetUnderlying());
}

TEST_CASE("Floating point", "[StrongTypedef]") {
    using Id = common::types::StrongTypedef<double, struct IdTag>;

    double id_dbl = 12.34f;
    Id id;
    id = id_dbl;

    Id id2(id_dbl);

    CHECK(id == id2);
    CHECK(id.GetUnderlying() == id_dbl);
    CHECK(id.GetUnderlying() == id2.GetUnderlying());
}

TEST_CASE("User defined", "[StrongTypedef]") {
    using Id = common::types::StrongTypedef<Custom, struct IdTag>;

    Custom id_custom{ 12, "34", true };
    Id id;
    id = id_custom;

    Id id2(id_custom);

    CHECK(id == id2);
    CHECK(id.GetUnderlying() == id_custom);
    CHECK(id.GetUnderlying() == id2.GetUnderlying());
}

TEST_CASE("Containers", "[StrongTypedef]") {
    using Id = common::types::StrongTypedef<Custom, struct IdTag>;

    std::vector<Id> vector{};
    std::unordered_map<Id, Id> map{};
    std::unordered_set<Id> set{};
}

} // namespace common::tests::types