#pragma once

#include <chrono>

namespace oryx::chrono {

using NowClock = std::chrono::system_clock;

inline auto Now() { return NowClock::now(); }

inline auto NowMillis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(Now().time_since_epoch()).count();
}

inline auto NowSecs() {
    using namespace std::chrono;
    return duration_cast<seconds>(Now().time_since_epoch()).count();
}

}  // namespace oryx::chrono