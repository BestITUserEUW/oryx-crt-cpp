#include "doctest.hpp"

#include <chrono>
#include <thread>

#include <oryx/chrono/stopwatch.hpp>

using namespace oryx::chrono;

namespace {
constexpr std::chrono::milliseconds kErrorMargin(100);

struct MockClock {
    using duration = std::chrono::nanoseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

    static constexpr bool is_steady = true;

    static auto now() noexcept -> time_point { return now_; }

    static time_point now_;
};

MockClock::time_point MockClock::now_ = {};

}  // namespace

TEST_CASE("Stopwatch should measure elapsed time") {
    std::chrono::milliseconds sleep_time(100);
    Stopwatch sw{};
    std::this_thread::sleep_for(sleep_time);
    auto elapsed = sw.ElapsedMs();
    CHECK_GE(elapsed, sleep_time);
    CHECK_LT(elapsed, sleep_time + kErrorMargin);
}

TEST_CASE("Reset calls clock now") {
    details::StopwatchImpl<MockClock> sw{};
    const auto tp = MockClock::time_point{std::chrono::seconds(5)};
    MockClock::now_ = tp;
    sw.Reset();
    CHECK_EQ(sw.GetStart(), tp);
}

TEST_CASE("Elapsed and elapsed ms subtracts now from start") {
    details::StopwatchImpl<MockClock> sw{MockClock::time_point{std::chrono::seconds(5)}};
    const auto tp = MockClock::time_point{std::chrono::seconds(10)};
    MockClock::now_ = tp;
    CHECK_EQ(sw.ElapsedMs(), std::chrono::milliseconds(5000));

    MockClock::now_ = tp;
    CHECK_EQ(sw.Elapsed(), std::chrono::nanoseconds(5000000000));
}