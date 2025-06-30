#include "doctest.hpp"

#include <string>

#include <oryx/error.hpp>

namespace oryx {

TEST_CASE("What you give is what you get") {
    std::string what{"Give me that back"};
    Error error{what};
    CHECK_EQ(what, error.what());
}

}  // namespace oryx