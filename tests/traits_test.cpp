#include "doctest.hpp"

#include <functional>
#include <mutex>
#include <oryx/traits.hpp>

using namespace oryx;

namespace {

template <traits::BasicLockable Mtx>
struct Test {};

template <typename T>
struct is_basic_lockable : std::false_type {};

template <traits::BasicLockable T>
struct is_basic_lockable<T> : std::true_type {};

}  // namespace

TEST_CASE("std::function satisfies is_std_function") {
    CHECK(traits::is_std_function<std::function<void()>>{});
    CHECK(traits::is_std_function_v<std::function<void()>>);
}

TEST_CASE("lambda does not satisfy is_std_function") {
    CHECK_FALSE(traits::is_std_function_v<decltype([]() -> void { int x; })>);
}

TEST_CASE("std::mutex satisfies basic lockable") { CHECK(is_basic_lockable<std::mutex>{}); }

TEST_CASE("std::function does not satisfy basic lockable") { CHECK_FALSE(is_basic_lockable<std::function<void()>>{}); }