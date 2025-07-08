#pragma once

#include <chrono>

#include <oryx/chrono/stopwatch.hpp>

// TODO: Replace this with a CycleTimeController and add a MakeFrameRateController which just calculates the target
// cycle duration like in the current constructor. Then this can also be used to hold a specific cycle time like we
// already to in PathFindingCpp

namespace oryx::chrono {

class FrameRateController {
public:
    explicit FrameRateController(int target_fps);
    auto Sleep() -> bool;

private:
    const std::chrono::milliseconds target_frame_duration_;
    Stopwatch sw_;
};

}  // namespace oryx::chrono