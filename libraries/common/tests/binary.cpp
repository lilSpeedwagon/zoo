#include <chrono>
#include <string>

#include <catch2/catch.hpp>

#include <common/include/binary.hpp>


// Tests from this section cause a side-effect:
// temporary file for IO operations is created.

namespace common::tests::binary {

namespace {

static const char* kFileName = "./test_temp.bin";

void CheckString(const std::string& str, const char* expected) {
    CHECK(str == std::string(expected));
}

} // namespace

TEST_CASE("StringsIO", "[Binary]") {
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

TEST_CASE("IntegersIO", "[Binary]") {
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

TEST_CASE("vector", "[Binary]") {
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

TEST_CASE("chrono", "[Binary]") {
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

} // namespace common::tests::binary
