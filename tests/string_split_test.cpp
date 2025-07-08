#include "doctest.hpp"

#include <cstdio>
#include <oryx/string_split.hpp>

namespace oryx {

TEST_CASE("split string -") {
    const auto split = StringSplit("this-will-be-split", '-');
    REQUIRE_FALSE(split.empty());
    REQUIRE(split.size() == 4);

    CHECK(split[0] == "this");
    CHECK(split[1] == "will");
    CHECK(split[2] == "be");
    CHECK(split[3] == "split");
}

TEST_CASE("split string space") {
    const auto split = StringSplit("hello-world Split MEPLEASE", ' ');
    REQUIRE_FALSE(split.empty());
    REQUIRE(split.size() == 3);

    CHECK(split[0] == "hello-world");
    CHECK(split[1] == "Split");
    CHECK(split[2] == "MEPLEASE");
}

TEST_CASE("nothing will be split") {
    const auto split = StringSplit("this will not be split", '+');
    REQUIRE(split.empty());
}

}  // namespace oryx