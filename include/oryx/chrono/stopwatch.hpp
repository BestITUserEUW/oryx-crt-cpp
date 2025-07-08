#pragma once

#include <chrono>

#include <oryx/types.hpp>

namespace oryx::chrono {

class Stopwatch {
public:
    using clock = std::chrono::steady_clock;

    Stopwatch()
        : start_{clock::now()} {}

    Stopwatch(clock::time_point start)
        : start_(start) {}

    auto Elapsed() const -> std::chrono::duration<f64> { return std::chrono::duration<f64>(clock::now() - start_); }
    auto ElapsedMs() const -> std::chrono::milliseconds {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_);
    }
    void Reset() { start_ = clock::now(); }

    auto GetStart() const -> clock::time_point { return start_; }

private:
    clock::time_point start_;
};

}  // namespace oryx::chrono