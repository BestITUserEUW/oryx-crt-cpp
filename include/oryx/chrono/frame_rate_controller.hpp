#pragma once

#include <chrono>

#include <oryx/chrono/stopwatch.hpp>

namespace oryx::chrono {

class FrameRateController {
public:
    explicit FrameRateController(int target_fps);
    bool Sleep();

private:
    const std::chrono::milliseconds target_frame_duration_;
    Stopwatch sw_;
};

}  // namespace oryx::chrono