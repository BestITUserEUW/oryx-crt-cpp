#pragma once

#include <utility>

namespace oryx::crt {

template <class F>
class ScopeExit {
public:
    explicit ScopeExit(F&& fn)
        : fn_(std::forward<F>(fn)) {}

    ~ScopeExit() {
        if (is_owner_) {
            fn_();
        }
    }

    void Release() {
        is_owner_ = false;  // NOTE: Should this return the function object
    }

    ScopeExit(ScopeExit&& other) = delete;
    ScopeExit(const ScopeExit&) = delete;
    auto operator=(const ScopeExit&) -> ScopeExit& = delete;

private:
    F fn_;
    bool is_owner_{true};
};

}  // namespace oryx::crt