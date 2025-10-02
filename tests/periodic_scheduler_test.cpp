#include "doctest.hpp"
#include "chrono_mock_clock.hpp"

#include <oryx/periodic_scheduler.hpp>

using namespace oryx;
using Scheduler = detail::PeriodicSchedulerImpl<ChronoMockClock>;

TEST_CASE("") {}