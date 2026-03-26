#include "doctest.hpp"

#include <cstdint>
#include <limits>

#include <oryx/crt/narrow_cast.hpp>

using namespace oryx::crt;

TEST_CASE("same type conversions") {
    CHECK(Narrow<int, int>(0) == 0);
    CHECK(Narrow<int, int>(42) == 42);
    CHECK(Narrow<unsigned, unsigned>(123u) == 123u);
}

TEST_CASE("signed to signed") {
    CHECK(Narrow<int8_t, int>(127) == 127);
    CHECK(Narrow<int8_t, int>(-128) == -128);

    CHECK_FALSE(Narrow<int8_t, int>(128).has_value());
    CHECK_FALSE(Narrow<int8_t, int>(-129).has_value());
}

TEST_CASE("unsigned to unsigned") {
    CHECK(Narrow<uint8_t, unsigned>(255u) == 255u);
    CHECK_FALSE(Narrow<uint8_t, unsigned>(256u).has_value());
}

TEST_CASE("signed to unsigned") {
    CHECK(Narrow<unsigned, int>(0) == 0u);
    CHECK(Narrow<unsigned, int>(42) == 42u);

    CHECK_FALSE(Narrow<unsigned, int>(-1).has_value());
    CHECK_FALSE(Narrow<uint8_t, int>(std::numeric_limits<int>::max()).has_value());
}

TEST_CASE("unsigned to signed") {
    CHECK(Narrow<int, unsigned>(0u) == 0);
    CHECK(Narrow<int, unsigned>(42u) == 42);

    CHECK_FALSE(Narrow<int8_t, unsigned>(128u).has_value());
}

TEST_CASE("boundary conditions") {
    CHECK(Narrow<int, unsigned>(static_cast<unsigned>(std::numeric_limits<int>::max())) ==
          std::numeric_limits<int>::max());

    CHECK_FALSE(Narrow<int, unsigned>(static_cast<unsigned>(std::numeric_limits<int>::max()) + 1u).has_value());
}