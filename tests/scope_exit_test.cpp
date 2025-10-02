#include "doctest.hpp"

#include <oryx/scope_exit.hpp>

using namespace oryx;

TEST_CASE("ScopeExit calls fn on scope exit") {
    bool flag{};
    {
        ScopeExit se{[&flag] { flag = true; }};
    }

    CHECK(flag);
}

TEST_CASE("ScopeExit does not call fn if released prior") {
    bool flag{};
    {
        ScopeExit se{[&flag] { flag = true; }};
        se.Release();
    }

    CHECK_FALSE(flag);
}
