#include <chrono>
#include <list>
#include <optional>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include <common/include/binary.hpp>
#include <common/include/strong_typedef.hpp>


// Tests from this section cause a side-effect:
// temporary file for IO operations is created.

namespace common::tests::binary {

namespace {

static const char* kFileName = "./test_temp.bin";

void CheckString(const std::string& str, const char* expected) {
    CHECK(str == std::string(expected));
}

class BinaryTestFixture {
public:
    BinaryTestFixture() {
        // cleanup temp file if exist
        auto is_deleted = std::filesystem::remove(std::filesystem::path(kFileName));
    }
};

} // namespace

TEST_CASE_METHOD(BinaryTestFixture, "StringsIO", "[Binary]") {
    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << "hello";
        wrapper_out << "world";
    }
    
    std::string s1{};
    std::string s2{};
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        wrapper_in >> s1;
        wrapper_in >> s2;
    }

    CheckString(s1, "hello");
    CheckString(s2, "world");
}

TEST_CASE_METHOD(BinaryTestFixture, "IntegersIO", "[Binary]") {
    int signed_int = 123;
    long signed_long = 12356789;
    char signed_char = 127;
    int negative_int = -123;
    long negative_long = -123456789;
    char negative_char = -127;
    unsigned unsigned_int = 123;
    unsigned long unsigned_long = 12356789;
    unsigned char unsigned_char = 255;

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << signed_int << signed_long << signed_char;
        wrapper_out << negative_int << negative_long << negative_char;
        wrapper_out << unsigned_int << unsigned_long << unsigned_char;
    }

    {
        common::binary::BinaryInStream wrapper_in(kFileName);

        // signed
        int n1;
        long n2;
        char n3;
        wrapper_in >> n1 >> n2 >> n3;
        CHECK(n1 == signed_int);
        CHECK(n2 == signed_long);
        CHECK(n3 == signed_char);

        // negative
        int n4;
        long n5;
        char n6;
        wrapper_in >> n4 >> n5 >> n6;
        CHECK(n4 == negative_int);
        CHECK(n5 == negative_long);
        CHECK(n6 == negative_char);

        // unsigned
        unsigned int n7;
        unsigned long n8;
        unsigned char n9;
        wrapper_in >> n7 >> n8 >> n9;
        CHECK(n7 == unsigned_int);
        CHECK(n8 == unsigned_long);
        CHECK(n9 == unsigned_char);
    }
}


TEST_CASE_METHOD(BinaryTestFixture, "FloatingPointIO", "[Binary]") {
    float float_positive = 123.456f;
    float float_negative = -123.456f;
    double double_positive = 123.456789;
    double double_negative = -123.456789;

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << float_positive << float_negative;
        wrapper_out << double_positive << double_negative;
    }

    {
        common::binary::BinaryInStream wrapper_in(kFileName);

        float n1;
        float n2;
        double n3;
        double n4;
        wrapper_in >> n1 >> n2;
        wrapper_in >> n3 >> n4;
        CHECK(n1 == float_positive);
        CHECK(n2 == float_negative);
        CHECK(n3 == double_positive);
        CHECK(n4 == double_negative);
    }
}

TEST_CASE_METHOD(BinaryTestFixture, "BooleanIO", "[Binary]") {
    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << true;
        wrapper_out << false;
    }
    
    bool b1;
    bool b2;
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        wrapper_in >> b1;
        wrapper_in >> b2;
    }

    CHECK(b1 == true);
    CHECK(b2 == false);
}

TEST_CASE_METHOD(BinaryTestFixture, "VectorIO", "[Binary]") {
    std::vector<int> numbers = {1, 2, 3};

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << numbers;
    }
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        std::vector<int> result;
        wrapper_in >> result;
        
        CHECK(numbers == result);
    }
}

TEST_CASE_METHOD(BinaryTestFixture, "ListIO", "[Binary]") {
    std::list<int> numbers = {1, 2, 3};

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << numbers;
    }
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        std::list<int> result;
        wrapper_in >> result;
        
        CHECK(numbers == result);
    }
}


TEST_CASE_METHOD(BinaryTestFixture, "ChronoIO", "[Binary]") {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << tp;
    }
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        std::chrono::system_clock::time_point result;
        wrapper_in >> result;
        
        CHECK(tp == result);
    }
}

TEST_CASE_METHOD(BinaryTestFixture, "OptionalIO", "[Binary]") {
    std::optional<int> int_opt_empty = std::nullopt;
    std::optional<int> int_opt = 123;
    std::optional<std::string> str_opt = "str";

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << int_opt_empty << int_opt << str_opt;
    }
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        
        std::optional<int> opt1;
        std::optional<int> opt2;
        std::optional<std::string> opt3;
        wrapper_in >> opt1 >> opt2 >> opt3;
        
        CHECK(opt1 == int_opt_empty);
        CHECK(opt2 == int_opt);
        CHECK(opt3 == str_opt);
    }
}

TEST_CASE_METHOD(BinaryTestFixture, "StrongTypedefIO", "[Binary]") {
    using IntT = common::types::StrongTypedef<int, struct IntTag>;
    using StrT = common::types::StrongTypedef<std::string, struct StrTag>;

    IntT int_val(123);
    StrT str_val("str");

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << int_val << str_val;
    }
    {
        common::binary::BinaryInStream wrapper_in(kFileName);
        
        IntT st1{};
        StrT st2{};
        wrapper_in >> st1 >> st2;
        
        CHECK(st1 == int_val);
        CHECK(st2 == str_val);
    }
}

TEST_CASE_METHOD(BinaryTestFixture, "SequentalWrite", "[Binary]") {
    std::vector<std::string> data = {
        "Hello ",
        "world",
        "!!!",
    };

    for (const auto& item : data) {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out.Seek(common::binary::BinaryStreamPosition::END);
        wrapper_out << item;
    }

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
    }

    common::binary::BinaryInStream wrapper_in(kFileName);
    for (const auto& item : data) {
        std::string str;
        wrapper_in >> str;
        CHECK(str == item);
    }
}

TEST_CASE_METHOD(BinaryTestFixture, "Rewrite", "[Binary]") {
    std::string str1 = "Halle";
    std::string str2 = "Hello";
    
    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << str1;
    }

    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out.Seek(common::binary::BinaryStreamPosition::BEGIN);
        wrapper_out << str2;
    }

    common::binary::BinaryInStream wrapper_in(kFileName);
    std::string result;
    wrapper_in >> result;
    CHECK(result == str2);
}

TEST_CASE_METHOD(BinaryTestFixture, "Truncate", "[Binary]") {
    std::string str1 = "Halle";
    std::string str2 = "Hello";
    
    {
        common::binary::BinaryOutStream wrapper_out(kFileName);
        wrapper_out << str1;
    }

    {
        common::binary::BinaryOutStream wrapper_out(kFileName, true);
        wrapper_out << str2;
    }

    common::binary::BinaryInStream wrapper_in(kFileName);
    std::string result;
    wrapper_in >> result;
    CHECK(result == str2);
}

} // namespace common::tests::binary
