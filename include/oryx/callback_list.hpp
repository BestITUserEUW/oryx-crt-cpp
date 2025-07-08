#pragma once

#include <utility>
#include <algorithm>
#include <functional>
#include <atomic>

#include "synchronized.hpp"

namespace oryx {

/**
 * @brief CallbackList is a thread safe list that stores callbacks, and can notify these callbacks.
 * @tparam Args
 */
template <class... Args>
class CallbackList {
public:
    using ID = uint64_t;
    /**
     * @brief Handle to subscription. Can be used to unsubscribe from notifications.
     */
    class Handle {
    public:
        Handle()
            : id_(0) {}

        auto IsValid() const -> bool { return id_ != 0; }
        auto id() const { return id_; }

    private:
        friend class CallbackList<Args...>;

        explicit Handle(ID id)
            : id_(id) {}

        ID id_;
    };

    using Callback = std::function<void(Args...)>;

    CallbackList() = default;

    auto Subscribe(const Callback& cb) -> Handle {
        auto handle = CreateHandle();
        subs_.wlock()->emplace_back(handle.id(), cb);
        return handle;
    }

    auto Subscribe(Callback&& cb) -> Handle {
        auto handle = CreateHandle();
        subs_.wlock()->emplace_back(handle.id(), std::forward<Callback>(cb));
        return handle;
    }

    void Unsubscribe(Handle& handle) {
        if (!handle.IsValid()) {
            return;
        }

        auto subs_ptr = subs_.wlock();
        auto it = std::ranges::find_if(*subs_ptr, [hid = handle.id()](ID id) { return id == hid; }, &Subscriber::id);
        if (it != subs_ptr->end()) {
            subs_ptr->erase(it);
            handle = {};
        }
    }

    void Notify(const Args&... args) const {
        for (auto& sub : *subs_.rlock()) {
            sub.cb(args...);
        }
    }

    void Clear() { subs_.wlock()->clear(); }
    auto IsEmpty() const { return subs_.rlock()->empty(); }
    auto Size() const { return subs_.rlock()->size(); }

private:
    struct Subscriber {
        ID id;
        Callback cb;
    };

    auto CreateHandle() -> Handle { return Handle(id_.fetch_add(1, std::memory_order_relaxed)); }

    std::atomic<ID> id_{1};
    synchronized<std::vector<Subscriber>> subs_{};
};

}  // namespace oryx
