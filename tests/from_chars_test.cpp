#include <oryx/crt/from_chars.hpp>

#include "doctest.hpp"

using namespace oryx::crt;

TEST_CASE("Parsing valid values") {
    CHECK(FromChars<int>("1").value() == 1);
    CHECK(FromChars<int>("-1").value() == -1);
    CHECK(FromChars<float>("1.2").value() == 1.2f);
    CHECK(FromChars<float>("-1.2").value() == -1.2f);
    CHECK(FromChars<double>("1.2").value() == 1.2);
    CHECK(FromChars<double>("-1.2").value() == -1.2);
    CHECK(FromChars<bool>("0").value() == false);
    CHECK(FromChars<bool>("1").value() == true);
}

TEST_CASE("Parsing invalid values") {
    CHECK_FALSE(FromChars<int>("why"));
    CHECK_FALSE(FromChars<float>("are"));
    CHECK_FALSE(FromChars<double>("you"));
    CHECK_FALSE(FromChars<bool>("reading"));
}