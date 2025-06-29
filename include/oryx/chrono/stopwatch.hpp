#pragma once

#include <chrono>

#include <oryx/types.hpp>

namespace oryx::chrono {

class Stopwatch {
public:
    using clock = std::chrono::steady_clock;

    Stopwatch()
        : start_tp_{clock::now()} {}

    auto Elapsed() const -> std::chrono::duration<f64> { return std::chrono::duration<f64>(clock::now() - start_tp_); }

    auto ElapsedMs() const -> std::chrono::milliseconds {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_tp_);
    }

    void Reset() { start_tp_ = clock::now(); }

private:
    std::chrono::time_point<clock> start_tp_;
};

}  // namespace oryx::chrono