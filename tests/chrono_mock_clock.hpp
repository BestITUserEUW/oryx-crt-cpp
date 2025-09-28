#pragma once

#include <chrono>

namespace oryx {

struct ChronoMockClock {
    using duration = std::chrono::nanoseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

    static constexpr bool is_steady = true;

    static auto now() noexcept -> time_point { return now_; }

    static time_point now_;
};

ChronoMockClock::time_point ChronoMockClock::now_ = {};

static_assert(std::chrono::is_clock_v<ChronoMockClock>, "ChronoMockClock does not satisfy chrono clock!");

}  // namespace oryx