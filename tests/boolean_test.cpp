#include "doctest.hpp"

#include <oryx/crt/boolean.hpp>

using namespace oryx::crt;

TEST_CASE("Default constructor is false") {
    Boolean b{};
    CHECK_FALSE(b);
}

TEST_CASE("AndThen chaining") {
    Boolean b = true;

    int counter{};
    auto increment = [&] {
        counter++;
        return true;
    };

    b.AndThen(increment).AndThen(increment).AndThen([] { return false; }).AndThen(increment);
    CHECK_EQ(counter, 2);
}

TEST_CASE("OrElse chaining") {
    Boolean b{};

    int counter{};
    auto increment = [&] {
        counter++;
        return false;
    };
    b.OrElse(increment).OrElse(increment).OrElse([] { return true; }).OrElse(increment);
    CHECK_EQ(counter, 2);
}

TEST_CASE("AndThen and OrElse") {
    Boolean b{};

    int counter{};
    auto increment = [&] {
        counter++;
        return false;
    };
    b.OrElse(increment).OrElse([] { return true; }).AndThen(increment);
    CHECK_EQ(counter, 2);
}

TEST_CASE("Transform") {
    Boolean b{};

    int r = b.Transform([](bool b) { return b ? 1 : 0; });
    CHECK_EQ(r, 0);
}

TEST_CASE("Operators") {
    Boolean b{};

    CHECK_EQ(Boolean{true}, Boolean{true});
    CHECK_EQ(Boolean{false} || Boolean{true}, Boolean{true});
    CHECK_EQ(Boolean{false} && Boolean{true}, Boolean{false});
}