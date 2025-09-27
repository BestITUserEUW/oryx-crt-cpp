#pragma once

#include <chrono>
#include <optional>

#include <oryx/scope_exit.hpp>
#include <oryx/chrono/stopwatch.hpp>

namespace oryx::chrono {

template <class Clock = std::chrono::steady_clock>
    requires std::chrono::is_clock_v<Clock>
class CycleTimer {
public:
    using Duration = std::chrono::milliseconds;

    explicit CycleTimer(Duration target)
        : target_(target) {}

    auto GetNextSleep() -> std::optional<Duration> {
        const auto elapsed = sw_.ElapsedMs();
        if (elapsed >= target_) {
            return std::nullopt;
        }

        return Duration(target_ - elapsed);
    }

    void Reset() { sw_.Reset(); }

    auto target_cycle_time() const -> Duration { return target_; }

    static auto MakeAutoResetGuard(CycleTimer<Clock>& timer) {
        return ScopeExit{[&timer] { timer.Reset(); }};
    };

    static auto MakeFrameRateTimer(int target_fps) { return CycleTimer<Clock>{Duration{1000 / target_fps}}; }

private:
    const Duration target_;
    details::StopwatchImpl<Clock> sw_{};
};

}  // namespace oryx::chrono