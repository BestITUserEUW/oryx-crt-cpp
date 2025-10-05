#include "doctest.hpp"

#include <chrono>

#include <oryx/crt/stopwatch.hpp>

#include "chrono_mock_clock.hpp"

using namespace oryx::crt;

TEST_CASE("Reset calls clock now") {
    details::StopwatchImpl<oryx::crt::ChronoMockClock> sw{};
    const auto tp = oryx::crt::ChronoMockClock::time_point{std::chrono::seconds(5)};
    oryx::crt::ChronoMockClock::set_time(tp);
    sw.Reset();
    CHECK_EQ(sw.GetStart(), tp);
}

TEST_CASE("Elapsed and elapsed ms subtracts now from start") {
    details::StopwatchImpl<oryx::crt::ChronoMockClock> sw{
        oryx::crt::ChronoMockClock::time_point{std::chrono::seconds(5)}};
    const auto tp = oryx::crt::ChronoMockClock::time_point{std::chrono::seconds(10)};
    oryx::crt::ChronoMockClock::set_time(tp);
    CHECK_EQ(sw.ElapsedMs(), std::chrono::milliseconds(5000));

    oryx::crt::ChronoMockClock::set_time(tp);
    CHECK_EQ(sw.Elapsed(), std::chrono::nanoseconds(5000000000));
}