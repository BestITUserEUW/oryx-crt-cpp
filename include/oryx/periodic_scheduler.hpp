#pragma once

#include <limits>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>
#include <functional>
#include <mutex>
#include <stop_token>

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
            scheduler_.reset();
        }

        std::weak_ptr<PeriodicSchedulerImpl> scheduler_{};
        TaskID id_{kTaskIDMax};
    };

    auto Schedule(TaskFn&& task, Duration interval, TaskStopPolicy stop_policy = TaskStopPolicy::kWaitForCompletion)
        -> TaskHandle {
        std::unique_lock lock{mtx_};
        TaskID id = task_counter_++;
        tasks_.emplace_back(std::move(task), id, stop_policy, interval, std::future<void>(), Clock::now());
        lock.unlock();
        cv_.notify_all();
        return TaskHandle(std::enable_shared_from_this<PeriodicSchedulerImpl<Clock>>::weak_from_this(), id);
    }

    auto GetNumTasks() const -> size_t {
        std::lock_guard lock{mtx_};
        return tasks_.size();
    }

    auto GetTaskCounter() const -> size_t {
        std::lock_guard lock{mtx_};
        return task_counter_;
    }

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
          mtx_(),
          cv_(),
          task_counter_(),
          worker_(&PeriodicSchedulerImpl<Clock>::ScheduleLoop, this) {}

    auto StopTask(TaskHandle& handle) -> bool {
        std::unique_lock lock{mtx_};
        auto it = std::ranges::find_if(tasks_, [&handle](const auto& task) { return handle.id_ == task.id; });
        if (it == tasks_.end()) {
            return false;
        }

        auto task = std::move(*it);
        tasks_.erase(it);
        lock.unlock();

        if (task.stop_policy == TaskStopPolicy::kWaitForCompletion) {
            if (task.promise.valid()) {
                task.promise.wait();
            }
        }
        handle.Reset();
        return true;
    }

    void ScheduleLoop(const std::stop_token& stoken) {
        std::stop_callback scb{stoken, [this] { cv_.notify_all(); }};

        while (!stoken.stop_requested()) {
            std::unique_lock lock{mtx_};
            if (tasks_.empty()) {
                cv_.wait(lock, stoken, [this] { return !tasks_.empty(); });
                continue;
            }

            auto now = Clock::now();
            auto next_wake_time = TimePoint::max();
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
                auto id = task_counter_;
                cv_.wait_until(lock, stoken, next_wake_time, [this, id]() { return id < task_counter_; });
            }
        }
    }

    ThreadPool& pool_;
    std::vector<Task> tasks_;
    mutable std::mutex mtx_;
    std::condition_variable_any cv_;
    TaskID task_counter_;
    std::jthread worker_;
};

}  // namespace detail

using PeriodicScheduler = detail::PeriodicSchedulerImpl<std::chrono::steady_clock>;
using PeriodicSchedulerPtr = std::shared_ptr<PeriodicScheduler>;

}  // namespace oryx