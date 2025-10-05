#include "doctest.hpp"

#include <string>

#include <oryx/crt/error.hpp>

using namespace oryx::crt;

TEST_CASE("What you give is what you get") {
    std::string what{"Give me that back"};
    Error error{what};
    CHECK_EQ(what, error.what());
}
