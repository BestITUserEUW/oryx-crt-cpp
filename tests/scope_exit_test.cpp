#include "doctest.hpp"

#include <oryx/crt/scope_exit.hpp>

using namespace oryx::crt;

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
