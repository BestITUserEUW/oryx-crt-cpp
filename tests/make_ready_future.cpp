#include "doctest.hpp"

#include <oryx/crt/make_ready_future.hpp>

using namespace oryx::crt;

TEST_CASE("future should not throw when calling get()") {
    auto fut = MakeReadyFuture(42);
    CHECK_NOTHROW(fut.get());
}

TEST_CASE("void future should not throw when calling get()") {
    auto fut = MakeReadyFuture();
    CHECK_NOTHROW(fut.get());
}

TEST_CASE("get() should return what was passed") {
    auto fut = MakeReadyFuture(42);
    CHECK(fut.valid());
    CHECK(fut.get() == 42);
}