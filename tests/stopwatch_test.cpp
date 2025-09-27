#include "doctest.hpp"

#include <chrono>

#include <oryx/chrono/stopwatch.hpp>

#include "chrono_mock_clock.hpp"

using namespace oryx::chrono;

TEST_CASE("Reset calls clock now") {
    details::StopwatchImpl<oryx::ChronoMockClock> sw{};
    const auto tp = oryx::ChronoMockClock::time_point{std::chrono::seconds(5)};
    oryx::ChronoMockClock::now_ = tp;
    sw.Reset();
    CHECK_EQ(sw.GetStart(), tp);
}

TEST_CASE("Elapsed and elapsed ms subtracts now from start") {
    details::StopwatchImpl<oryx::ChronoMockClock> sw{oryx::ChronoMockClock::time_point{std::chrono::seconds(5)}};
    const auto tp = oryx::ChronoMockClock::time_point{std::chrono::seconds(10)};
    oryx::ChronoMockClock::now_ = tp;
    CHECK_EQ(sw.ElapsedMs(), std::chrono::milliseconds(5000));

    oryx::ChronoMockClock::now_ = tp;
    CHECK_EQ(sw.Elapsed(), std::chrono::nanoseconds(5000000000));
}