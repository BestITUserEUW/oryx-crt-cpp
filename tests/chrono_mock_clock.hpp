#pragma once

#include <chrono>
#include <mutex>

namespace oryx::crt {

struct ChronoMockClock {
public:
    using duration = std::chrono::milliseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<ChronoMockClock>;

    static constexpr bool is_steady = true;

    static auto now() noexcept -> time_point {
        std::lock_guard lock{mtx_};
        return current_time;
    }
    static void set_time(time_point t) {
        std::lock_guard lock{mtx_};
        current_time = t;
    }
    static void advance(duration d) {
        std::lock_guard lock{mtx_};
        current_time += d;
    }
    static void reset() {
        std::lock_guard lock{mtx_};
        current_time = time_point{};
    }

private:
    static inline std::mutex mtx_{};
    static inline time_point current_time{};
};

static_assert(std::chrono::is_clock_v<ChronoMockClock>, "ChronoMockClock does not satisfy chrono clock!");

}  // namespace oryx::crt