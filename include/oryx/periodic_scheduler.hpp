#pragma once

#include <atomic>
#include <limits>
#include <optional>
#include <string>
#include <memory>
#include <chrono>
#include <utility>
#include <vector>
#include <functional>
#include <mutex>

#include "thread_pool.hpp"

namespace oryx {
namespace detail {

template <class Clock>
    requires std::chrono::is_clock_v<Clock>
class PeriodicSchedulerImpl : public std::enable_shared_from_this<PeriodicSchedulerImpl<Clock>> {
public:
    using TaskFn = std::function<void()>;
    using TaskName = std::string;
    using TaskID = uint64_t;
    using Duration = std::chrono::milliseconds;
    using TimePoint = Clock::time_point;
    using ThreadPool = BS::thread_pool<BS::tp::none>;

    static constexpr TaskID kTaskIDMax = std::numeric_limits<TaskID>::max();

    enum class TaskStopPolicy : uint8_t {
        kWaitForCompletion,  // wait for promise to finish
        kSkipWait            // don't wait, let it run out
    };

    class TaskHandle {
    public:
        TaskHandle() = default;

        // Move constructible
        TaskHandle(TaskHandle&& other) noexcept
            : scheduler_(std::move(other.scheduler_)),
              id_(std::exchange(other.id_, kTaskIDMax)) {}

        auto operator=(TaskHandle&& other) noexcept -> TaskHandle& {
            if (this != &other) {
                Stop();
                scheduler_ = std::move(other.scheduler_);
                id_ = std::exchange(other.id_, kTaskIDMax);
            }
            return *this;
        }

        TaskHandle(const TaskHandle&) = delete;
        auto operator=(const TaskHandle&) -> TaskHandle& = delete;

        ~TaskHandle() { Stop(); }

        auto Stop() -> bool {
            if (!IsValid()) {
                return false;
            }

            if (auto sched = scheduler_.lock(); sched) {
                bool stopped = sched->StopTask(*this);
                id_ = kTaskIDMax;
                return stopped;
            }
            return false;
        }

        auto IsValid() const -> bool { return id_ != kTaskIDMax; }
        auto id() const -> TaskID { return id_; }

    private:
        friend class PeriodicSchedulerImpl<Clock>;

        TaskHandle(std::weak_ptr<PeriodicSchedulerImpl> scheduler, TaskID id)
            : scheduler_(std::move(scheduler)),
              id_(id) {}

        void Reset() {
            id_ = kTaskIDMax;
            scheduler_ = nullptr;
        }

        std::weak_ptr<PeriodicSchedulerImpl> scheduler_{};
        TaskID id_{kTaskIDMax};
    };

    auto Schedule(TaskFn&& task, Duration interval, TaskStopPolicy stop_policy = TaskStopPolicy::kWaitForCompletion)
        -> TaskHandle {
        TaskID id = task_counter_.fetch_add(1, std::memory_order_relaxed);
        {
            std::lock_guard lock{tasks_mtx_};
            tasks_.emplace_back(std::move(task), id, stop_policy, interval, std::future<void>{}, Clock::now());
        }
        cv_.notify_all();
        return TaskHandle(std::enable_shared_from_this<PeriodicSchedulerImpl<Clock>>::weak_from_this(), id);
    }

    auto GetNumTasks() const -> size_t {
        std::lock_guard lock{tasks_mtx_};
        return tasks_.size();
    }

    auto TotalNumTasks() const -> size_t { return task_counter_.load(std::memory_order_relaxed); }

    static auto Create(ThreadPool& pool) -> std::shared_ptr<PeriodicSchedulerImpl> {
        return std::shared_ptr<PeriodicSchedulerImpl>(new PeriodicSchedulerImpl(pool));
    }

private:
    friend class TaskHandle;

    struct Task {
        TaskFn task;
        TaskID id;
        TaskStopPolicy stop_policy;
        Duration interval;
        std::future<void> promise;
        TimePoint next_execution;
    };

    PeriodicSchedulerImpl(ThreadPool& pool)
        : pool_(pool),
          tasks_(),
          tasks_mtx_(),
          cv_(),
          task_counter_(),
          worker_(&PeriodicSchedulerImpl<Clock>::ScheduleLoop, this) {}

    auto StopTask(TaskHandle& handle) -> bool {
        std::optional<Task> task{};
        {
            std::lock_guard lock{tasks_mtx_};
            auto it = std::ranges::find_if(tasks_, [&handle](const auto& t) { return handle.id_ == t.id; });
            if (it != tasks_.end()) {
                task.emplace(std::move(*it));
                tasks_.erase(it);
            }
        }

        if (!task) {
            return false;
        }

        if (task->stop_policy == TaskStopPolicy::kWaitForCompletion) {
            if (task->promise.valid()) {
                task->promise.wait();
            }
        }

        handle.Reset();
        return true;
    }

    void ScheduleLoop(const std::stop_token& stoken) {
        while (!stoken.stop_requested()) {
            std::unique_lock lock{tasks_mtx_};
            if (tasks_.empty()) {
                cv_.wait(lock, stoken, [&] { return stoken.stop_requested() || !tasks_.empty(); });
                continue;
            }

            auto now = Clock::now();
            TimePoint next_wake_time = TimePoint::max();
            for (Task& task : tasks_) {
                if (now >= task.next_execution) {
                    bool is_running = task.promise.valid() &&
                                      task.promise.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
                    if (!is_running) {
                        task.promise = pool_.submit_task(task.task);
                    }
                    task.next_execution = now + task.interval;
                }

                next_wake_time = std::min(next_wake_time, task.next_execution);
            }

            if (next_wake_time != TimePoint::max()) {
                cv_.wait_until(lock, stoken, next_wake_time,
                               [&] { return stoken.stop_requested() || !tasks_.empty(); });
            }
        }
    }

    ThreadPool& pool_;
    std::vector<Task> tasks_;
    std::mutex tasks_mtx_;
    std::condition_variable_any cv_;
    std::atomic<TaskID> task_counter_;
    std::jthread worker_;
};

}  // namespace detail

using PeriodicScheduler = detail::PeriodicSchedulerImpl<std::chrono::steady_clock>;
using PeriodicSchedulerPtr = std::shared_ptr<PeriodicScheduler>;

}  // namespace oryx