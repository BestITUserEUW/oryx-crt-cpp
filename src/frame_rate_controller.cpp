#include <oryx/chrono/frame_rate_controller.hpp>

#include <thread>

using namespace std::chrono;

namespace oryx::chrono {

FrameRateController::FrameRateController(int target_fps)
    : target_frame_duration_(1000 / target_fps),
      sw_() {}

auto FrameRateController::Sleep() -> bool {
    const auto elapsed = sw_.ElapsedMs();
    bool slept{};
    if (elapsed < target_frame_duration_) {
        const auto sleep_time = milliseconds(target_frame_duration_ - elapsed);
        std::this_thread::sleep_for(sleep_time);
        slept = true;
    }
    sw_.Reset();
    return slept;
}

}  // namespace oryx::chrono