#pragma once

#include <chrono>
#include <concepts>
#include <thread>
#include <type_traits>

#include <oryx/types.hpp>

namespace oryx::retry {

struct ExponentialConfig {
    using Duration = std::chrono::milliseconds;

    Duration start_backoff;
    Duration max_backoff;
    u64 max_retries;
};

template <std::invocable F, std::predicate Predicate>
inline auto ExponentialBackoff(ExponentialConfig config, F &&retryable, Predicate &&predicate)
    -> std::invoke_result_t<F> {
    using Result = std::invoke_result_t<F>;

    auto current_backoff = config.start_backoff;
    u64 num_retries{};
    Result result;

    while (!predicate() && num_retries < config.max_retries) {
        result = retryable();
        if (result) {
            break;
        }

        if (predicate()) return result;
        std::this_thread::sleep_for(current_backoff);
        current_backoff = current_backoff * 2 > config.max_backoff ? config.max_backoff : current_backoff * 2;
        num_retries++;
    }
    return result;
}

template <std::invocable F>
inline auto ExponentialBackoff(ExponentialConfig config, F &&retryable) -> std::invoke_result_t<F> {
    return ExponentialBackoff(config, std::forward<F>(retryable), [] { return false; });
}

template <std::invocable F>
inline auto ExponentialBackoff(ExponentialConfig config, F &&retryable, const std::stop_token &stoken)
    -> std::invoke_result_t<F> {
    return ExponentialBackoff(config, std::forward<F>(retryable), [&stoken] { return stoken.stop_requested(); });
}

}  // namespace oryx::retry