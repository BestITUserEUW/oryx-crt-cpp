#include "doctest.hpp"
#include "chrono_mock_clock.hpp"

#include <atomic>

#include <oryx/periodic_scheduler.hpp>
#include <oryx/chrono/stopwatch.hpp>

using namespace oryx;
using namespace std::chrono_literals;

namespace {

using Scheduler = detail::PeriodicSchedulerImpl<ChronoMockClock>;

template <typename Predicate>
auto WaitForCondition(Predicate pred, std::chrono::milliseconds timeout = 5000ms) -> bool {
    chrono::Stopwatch sw{};
    while (!pred()) {
        if (sw.ElapsedMs() > timeout) {
            return false;
        }
        std::this_thread::sleep_for(1ms);
    }
    return true;
}

}  // namespace

TEST_CASE("PeriodicScheduler basic functionality") {
    ChronoMockClock::reset();
    Scheduler::ThreadPool pool(1);

    auto scheduler = Scheduler::Create(pool);

    SUBCASE("Schedule task and verify initial state") {
        std::atomic<int> counter = 0;

        auto handle = scheduler->Schedule([&]() { counter++; }, 100ms);

        CHECK(handle.IsValid());
        CHECK(scheduler->GetNumTasks() == 1);
        CHECK(scheduler->GetTaskCounter() == 1);
    }

    SUBCASE("Task execution counting") {
        std::atomic<int> execution_count = 0;

        auto handle = scheduler->Schedule([&]() { execution_count++; }, 50ms);

        ChronoMockClock::advance(100ms);
        CHECK(WaitForCondition([&]() { return execution_count > 0; }));
        CHECK(execution_count >= 1);

        ChronoMockClock::advance(100ms);
        CHECK(WaitForCondition([&]() { return execution_count > 1; }));
        CHECK(execution_count >= 2);
    }

    SUBCASE("Stop task with kWaitForCompletion") {
        std::atomic<int> counter = 0;
        std::atomic<bool> task_running = false;

        auto handle = scheduler->Schedule(
            [&]() {
                task_running = true;
                std::this_thread::sleep_for(10ms);
                counter++;
                task_running = false;
            },
            100ms, Scheduler::TaskStopPolicy::kWaitForCompletion);

        ChronoMockClock::advance(100ms);
        CHECK(WaitForCondition([&]() { return counter > 0; }));

        bool stopped = handle.Stop();
        CHECK(stopped);
        CHECK_FALSE(task_running);
        CHECK(scheduler->GetNumTasks() == 0);
    }

    SUBCASE("Stop task with kSkipWait") {
        std::atomic<int> counter = 0;

        auto handle = scheduler->Schedule([&]() { counter++; }, 100ms, Scheduler::TaskStopPolicy::kSkipWait);

        bool stopped = handle.Stop();
        CHECK(stopped);
        CHECK(scheduler->GetNumTasks() == 0);
    }

    SUBCASE("Multiple tasks scheduling") {
        ChronoMockClock::reset();
        std::atomic<int> counter1 = 0, counter2 = 0;

        auto handle1 = scheduler->Schedule([&]() { counter1++; }, 50ms);
        auto handle2 = scheduler->Schedule([&]() { counter2++; }, 100ms);

        CHECK(scheduler->GetNumTasks() == 2);
        CHECK(scheduler->GetTaskCounter() == 2);

        WaitForCondition([&]() { return counter1 == 1 && counter2 == 1; });
        pool.wait();

        ChronoMockClock::advance(100ms);
        CHECK(WaitForCondition([&]() { return counter1 == 2 && counter2 == 2; }));
        pool.wait();

        ChronoMockClock::advance(50ms);
        CHECK(WaitForCondition([&]() { return counter1 == 3 && counter2 == 2; }));
        pool.wait();

        ChronoMockClock::advance(50ms);
        CHECK(WaitForCondition([&]() { return counter1 == 4 && counter2 == 3; }));
    }

    SUBCASE("TaskHandle move semantics") {
        std::atomic<int> counter = 0;

        auto handle1 = scheduler->Schedule([&]() { counter++; }, 50ms);
        auto id = handle1.id();

        auto handle2 = std::move(handle1);

        CHECK_FALSE(handle1.IsValid());
        CHECK(handle2.IsValid());
        CHECK(handle2.id() == id);
        CHECK(scheduler->GetNumTasks() == 1);

        handle2.Stop();
        CHECK(scheduler->GetNumTasks() == 0);
    }

    SUBCASE("Invalid handle operations") {
        auto handle = Scheduler::TaskHandle();

        CHECK_FALSE(handle.IsValid());
        CHECK_FALSE(handle.Stop());
    }

    SUBCASE("TaskHandle destructor stops task") {
        std::atomic<int> counter = 0;

        {
            auto handle = scheduler->Schedule([&]() { counter++; }, 50ms);
            CHECK(scheduler->GetNumTasks() == 1);
        }

        CHECK(scheduler->GetNumTasks() == 0);
    }
}