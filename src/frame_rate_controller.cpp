#include <oryx/chrono/frame_rate_controller.hpp>

#include <thread>

using namespace std::chrono;

namespace oryx::chrono {

FrameRateController::FrameRateController(int target_fps)
    : target_frame_duration_(1000 / target_fps),
      sw_() {}

auto FrameRateController::Sleep() -> bool {
    auto elapsed = sw_.ElapsedMs();

    if (elapsed < target_frame_duration_) {
        std::this_thread::sleep_for(milliseconds(target_frame_duration_ - elapsed));
        return true;
    }

    sw_.Reset();
    return false;
}

}  // namespace oryx::chrono