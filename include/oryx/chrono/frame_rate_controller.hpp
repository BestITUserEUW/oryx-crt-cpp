#pragma once

#include <chrono>

#include <oryx/chrono/stopwatch.hpp>

namespace oryx::chrono {

class [[deprecated("Depreacted in favor of MakeFrameRateTimer() from cycle_timer.hpp")]] FrameRateController {
public:
    explicit FrameRateController(int target_fps);
    auto Sleep() -> bool;

private:
    const std::chrono::milliseconds target_frame_duration_;
    Stopwatch sw_;
};

}  // namespace oryx::chrono