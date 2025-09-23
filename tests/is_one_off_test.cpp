#include "doctest.hpp"

#include <cstdint>

#include <oryx/is_one_of.hpp>

using namespace oryx;

namespace {

enum class Test : uint8_t { kProp1, kProp2, kProp3 };

}

TEST_CASE("IsOneOf boolean") {
    bool value = true;
    CHECK(IsOneOf(value, false, false, true, false));
    CHECK_FALSE(IsOneOf(value, false, false, false));

    value = false;
    CHECK(IsOneOf(value, true, true, false, true));
    CHECK_FALSE(IsOneOf(value, true, true, true));
}

TEST_CASE("IsOneOf enum") {
    Test value = Test::kProp1;
    CHECK(IsOneOf(value, Test::kProp2, Test::kProp1));
    CHECK_FALSE(IsOneOf(value, Test::kProp2, Test::kProp3));
}