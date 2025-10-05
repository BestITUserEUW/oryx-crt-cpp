#include "doctest.hpp"

#include <string>

#include <oryx/crt/synchronized.hpp>

using namespace oryx::crt;

TEST_CASE("UpdateGuard write / read lock works") {
    Synchronized<int> data{};
    data.WriteLock().Value() = 5;

    CHECK_EQ(data.WriteLock().Value(), 5);
}

TEST_CASE("UpdateGuard explicit unlock") {
    Synchronized<int> data;
    auto value = data.WriteLock();
    *value = 5;
    value.Unlock();

    data.WriteLock().Value() = 10;
    CHECK_EQ(data.ReadLock().Value(), 10);
}

TEST_CASE("UpdateGuard releases lock on scope exit") {
    Synchronized<std::string> data{};
    {
        auto value = data.WriteLock();
        value->append("Hello World");
    }

    CHECK_EQ(data.ReadLock().Value(), "Hello World");
}

TEST_CASE("Synchronized apply / visit works") {
    Synchronized<int> data{};
    data.Apply([](int& value) { value = 10; });
    CHECK_EQ(data.ReadLock().Value(), 10);

    int value = data.Visit([](const int& value) { return value; });
    CHECK_EQ(value, 10);
}

TEST_CASE("Synchronized emplace works") {
    Synchronized<std::string> data{};
    data.Emplace("Hello World");

    CHECK_EQ(data.ReadLock().Value(), "Hello World");
}

TEST_CASE("Synchronized exchange works") {
    Synchronized<std::string> data{};
    data.WriteLock().Value() = "Hello World";

    auto old = data.Exchange("New World");
    CHECK_EQ(old, "Hello World");
    CHECK_EQ(data.ReadLock().Value(), "New World");
}