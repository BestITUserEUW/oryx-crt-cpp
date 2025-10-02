#pragma once

#include <concepts>
#include <mutex>
#include <type_traits>
#include <utility>

#include <oryx/traits.hpp>

namespace oryx {

template <typename GuardedType, traits::BasicLockable MutexType = std::mutex>
class Synchronized;

template <typename GuardedType, traits::BasicLockable MutexType = std::mutex>
class UpdateGuard {
public:
    using value_type = GuardedType;
    using mutex_type = MutexType;

    explicit UpdateGuard(Synchronized<GuardedType, MutexType> &sv)
        : lock_{sv.mutex_},
          guarded_data_{&sv.guarded_data_} {}

    explicit UpdateGuard(const Synchronized<std::remove_const_t<GuardedType>, MutexType> &sv)
        : lock_{sv.mutex_},
          guarded_data_{&sv.guarded_data_} {}

    auto operator->() const noexcept -> GuardedType * { return guarded_data_; }
    auto operator*() const noexcept -> GuardedType & { return *guarded_data_; }

    auto Value() const noexcept -> GuardedType & { return *guarded_data_; }
    void Unlock() { return lock_.unlock(); }

private:
    std::unique_lock<MutexType> lock_;
    GuardedType *guarded_data_;
};

template <typename GuardedType, traits::BasicLockable MutexType>
class Synchronized {
public:
    using value_type = GuardedType;
    using mutex_type = MutexType;

    Synchronized(const Synchronized &) = delete;
    auto operator=(const Synchronized &) -> Synchronized & = delete;

    template <typename... Args>
    explicit Synchronized(Args &&...args)
        : guarded_data_{std::forward<Args>(args)...} {}

    auto WriteLock() { return UpdateGuard{*this}; }

    auto ReadLock() const { return UpdateGuard<const value_type, MutexType>{*this}; }

    auto Copy() const -> value_type
        requires(std::is_copy_constructible_v<value_type>)
    {
        std::lock_guard lock{mutex_};
        return guarded_data_;
    }

    template <typename F>
    auto Apply(F &&visitor) {
        std::lock_guard lock{mutex_};
        return visitor(guarded_data_);
    }

    template <typename F>
    auto Visit(F &&visitor) const {
        std::lock_guard lock{mutex_};
        return visitor(guarded_data_);
    }

    auto Exchange(value_type &&new_value) {
        std::lock_guard lock{mutex_};
        return std::exchange(guarded_data_, std::forward<value_type>(new_value));
    }

    template <typename... Args>
    void Emplace(Args &&...args) {
        std::lock_guard lock{mutex_};
        value_type temp(std::forward<Args>(args)...);
        guarded_data_.~value_type();
        new (&guarded_data_) value_type(std::move(temp));
    }

private:
    friend class UpdateGuard<GuardedType, MutexType>;
    friend class UpdateGuard<GuardedType const, MutexType>;

    mutable MutexType mutex_;
    GuardedType guarded_data_;
};
}  // namespace oryx