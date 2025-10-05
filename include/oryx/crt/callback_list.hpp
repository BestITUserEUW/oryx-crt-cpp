#pragma once

#include <utility>
#include <algorithm>
#include <functional>
#include <atomic>

#include "synchronized.hpp"

namespace oryx::crt {

/**
 * @brief CallbackList is a thread safe list that stores callbacks, and can notify these callbacks.
 * @tparam Args
 */
template <class... Args>
class CallbackList {
public:
    using ID = uint64_t;
    using Callback = std::function<void(Args...)>;

    static constexpr ID kIDMax = std::numeric_limits<ID>::max();
    /**
     * @brief Handle to subscription. Can be used to unsubscribe from notifications.
     */
    class Handle {
    public:
        Handle()
            : id_(kIDMax) {}

        auto IsValid() const -> bool { return id_ != kIDMax; }
        auto id() const { return id_; }

    private:
        friend class CallbackList<Args...>;

        explicit Handle(ID id)
            : id_(id) {}

        void Reset() { id_ = kIDMax; }

        ID id_;
    };

    CallbackList() = default;

    auto Subscribe(Callback&& cb) -> Handle {
        ID id = id_.fetch_add(1, std::memory_order_relaxed);
        subs_.Apply([&](auto& subs) { subs.emplace_back(id, std::forward<Callback>(cb)); });
        return Handle(id);
    }

    void Unsubscribe(Handle& handle) {
        if (!handle.IsValid()) {
            return;
        }

        subs_.Apply([&handle](auto& subs) {
            auto it = std::ranges::find(subs, handle.id_, &Subscriber::id);
            if (it != subs.end()) {
                subs.erase(it);
                handle.Reset();
            }
        });
    }

    void Notify(const Args&... args) const {
        subs_.Visit([&](auto& subs) {
            for (auto& sub : subs) sub.cb(args...);
        });
    }

    void Clear() {
        subs_.Apply([](auto& subs) { subs.clear(); });
    }

    auto IsEmpty() const {
        return subs_.Visit([](auto& subs) { return subs.empty(); });
    }

    auto Size() const {
        return subs_.Visit([](auto& subs) { return subs.size(); });
    }

private:
    struct Subscriber {
        ID id;
        Callback cb;
    };

    std::atomic<ID> id_{};
    Synchronized<std::vector<Subscriber>> subs_{};
};

}  // namespace oryx::crt
