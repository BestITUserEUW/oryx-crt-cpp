#pragma once

#include <chrono>

namespace oryx::chrono {
namespace details {

template <class Clock>
    requires std::chrono::is_clock_v<Clock>
class StopwatchImpl {
public:
    StopwatchImpl()
        : start_{Clock::now()} {}

    explicit StopwatchImpl(Clock::time_point start)
        : start_(start) {}

    auto Elapsed() const -> std::chrono::nanoseconds { return Clock::now() - start_; }
    auto ElapsedMs() const -> std::chrono::milliseconds {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start_);
    }
    void Reset() { start_ = Clock::now(); }

    auto GetStart() const { return start_; }

private:
    Clock::time_point start_;
};

}  // namespace details

using HighResolutionStopwatch = details::StopwatchImpl<std::chrono::high_resolution_clock>;
using Stopwatch = details::StopwatchImpl<std::chrono::steady_clock>;

}  // namespace oryx::chrono