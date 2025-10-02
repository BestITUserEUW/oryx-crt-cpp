#include "doctest.hpp"

#include <string>

#include <oryx/lazy_component.hpp>
#include <type_traits>

using namespace oryx;

namespace {

struct A {
    std::string message;
};

struct B {
    static inline int counter = 0;
    B() { counter++; }
};

}  // namespace

TEST_CASE("LazyComponent underlying value is initially not initialized") {
    LazyComponent<A> a{[]() -> A { return A("Hello World"); }};
    CHECK_FALSE(a.IsInitialized());
}

TEST_CASE("LazyComponent is initialized after first access") {
    LazyComponent<A> a{[]() -> A { return A("Hello World"); }};
    CHECK(a.Get());
    CHECK(a.IsInitialized());
}

TEST_CASE("Calling explicit Init on LazyComponent should init underlying value") {
    LazyComponent<A> a{[]() -> A { return A("Hello World"); }};
    a.Init();
    CHECK(a.Get());
    CHECK(a.IsInitialized());
}

TEST_CASE("Default constructible underlying values use default constructor if no factory is passed") {
    LazyComponent<A> a{};
    CHECK(a.Get());
    CHECK(a.IsInitialized());
}

TEST_CASE("Star operator and arrow operator are working as expected") {
    LazyComponent<A> a{};
    (*a).message = "Hello World";
    CHECK_EQ(a->message, "Hello World");
}

TEST_CASE("Object is only created once") {
    LazyComponent<B> b{};
    CHECK(b.Get());
    CHECK_EQ(B::counter, 1);
    CHECK(b.Get());
    CHECK_EQ(B::counter, 1);
}

TEST_CASE("Make lazy creates a lazy component") {
    auto a = MakeLazy([]() { return A("Some Value"); });
    CHECK(std::is_same<decltype(a), LazyComponent<A>>());
    CHECK_EQ(a->message, "Some Value");
}