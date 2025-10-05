#include "doctest.hpp"

#include <numeric>

#include <oryx/crt/callback_list.hpp>

using namespace oryx::crt;

TEST_CASE("New CallbackList should be empty") {
    CallbackList<int> cbv;
    CHECK(cbv.IsEmpty());
    CHECK_FALSE(cbv.Size());
}

TEST_CASE("User created handle should be invalid") {
    CallbackList<int>::Handle handle;
    REQUIRE_FALSE(handle.IsValid());
}

TEST_CASE("CallbackList with one item should not be empty") {
    CallbackList<int> cbv;
    cbv.Subscribe([](int) { return; });
    CHECK_FALSE(cbv.IsEmpty());
    CHECK(cbv.Size());
}

TEST_CASE("Subscribing gives valid handle") {
    CallbackList<int> cbv;
    auto handle = cbv.Subscribe([](int) { return; });
    REQUIRE(handle.IsValid());
}

TEST_CASE("Subscribing then unsubscribing results in invalid handle") {
    CallbackList<int> cbv;
    auto handle = cbv.Subscribe([](int) { return; });
    REQUIRE(handle.IsValid());
    cbv.Unsubscribe(handle);
    REQUIRE_FALSE(handle.IsValid());
}

TEST_CASE("Unsubscribing from invalid handle should be noop") {
    CallbackList<int> cbv;
    CallbackList<int>::Handle handle;
    cbv.Unsubscribe(handle);
    REQUIRE_FALSE(handle.IsValid());
    REQUIRE(cbv.IsEmpty());
}

TEST_CASE("Notifying subscriber works") {
    CallbackList<int> cbv;
    int received_value;
    cbv.Subscribe([&received_value](int val) { received_value = val; });
    cbv.Notify(5);
    REQUIRE(received_value == 5);
}

TEST_CASE("Subscribe / Unsubscribe multiple") {
    CallbackList<int> cbv;
    std::vector<CallbackList<int>::Handle> handles;
    handles.push_back(cbv.Subscribe([](int) { return; }));
    handles.push_back(cbv.Subscribe([](int) { return; }));
    handles.push_back(cbv.Subscribe([](int) { return; }));

    for (auto& handle : handles) {
        REQUIRE(handle.IsValid());
        cbv.Unsubscribe(handle);
        REQUIRE(!handle.IsValid());
    }
}

TEST_CASE("Subscribe / Unsubscribe notify multiple") {
    CallbackList<int> cbv;
    std::vector<int> vals{0, 0, 0};
    std::vector<CallbackList<int>::Handle> handles;
    handles.push_back(cbv.Subscribe([&ref = vals[0]](int val) { ref = val; }));
    handles.push_back(cbv.Subscribe([&ref = vals[1]](int val) { ref = val; }));
    handles.push_back(cbv.Subscribe([&ref = vals[2]](int val) { ref = val; }));

    for (const auto& handle : handles) {
        REQUIRE(handle.IsValid());
    }

    REQUIRE(cbv.Size() == 3);
    cbv.Notify(2);
    REQUIRE(std::accumulate(vals.begin(), vals.end(), 0) == 6);

    cbv.Unsubscribe(*std::next(handles.begin()));
    REQUIRE(cbv.Size() == 2);
    cbv.Notify(5);
    CHECK(std::accumulate(vals.begin(), vals.end(), 0) == 12);
    CHECK(vals[0] == 5);
    CHECK(vals[1] == 2);
    CHECK(vals[2] == 5);
}