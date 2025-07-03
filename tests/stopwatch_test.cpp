#include "doctest.hpp"

#include <thread>

#include <oryx/chrono/stopwatch.hpp>

using namespace oryx::chrono;

constexpr std::chrono::milliseconds kErrorMargin(100);

TEST_CASE("Stopwatch should measure elapsed time") {
    std::chrono::milliseconds sleep_time(100);
    Stopwatch sw{};
    std::this_thread::sleep_for(sleep_time);
    auto elapsed = sw.ElapsedMs();
    CHECK_GE(elapsed, sleep_time);
    CHECK_LT(elapsed, sleep_time + kErrorMargin);
}