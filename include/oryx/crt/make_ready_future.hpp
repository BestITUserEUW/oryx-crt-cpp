#pragma once

#include <future>

namespace oryx::crt {

template <typename T>
[[nodiscard]] auto MakeReadyFuture(T&& value) {
    std::promise<std::decay_t<T>> p;
    p.set_value(std::forward<T>(value));
    return p.get_future();
}

[[nodiscard]] inline auto MakeReadyFuture() {
    std::promise<void> p;
    p.set_value();
    return p.get_future();
}

}  // namespace oryx::crt