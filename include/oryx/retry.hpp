#pragma once

#include <chrono>
#include <thread>
#include <format>

#include <oryx/expected.hpp>
#include <oryx/error.hpp>
#include <oryx/types.hpp>

namespace oryx::retry {

enum class ErrorKind : uint8_t {
    kRetriesExhausted,
};

struct ExponentialConfig {
    std::string name;
    std::chrono::milliseconds start_backoff;
    std::chrono::milliseconds max_backoff;
    int max_retries;
};

template <typename T, typename F>
inline auto ExponentialBackoff(ExponentialConfig config, F &&retryable, const std::stop_token &stoken)
    -> expected<T, Error> {
    auto current_backoff = config.start_backoff;
    int num_retries = 0;
    expected<T, Error> result;

    while (!stoken.stop_requested() && num_retries < config.max_retries) {
        result = retryable();

        if (result) {
            break;
        }

        std::this_thread::sleep_for(current_backoff);
        current_backoff = current_backoff * 2 > config.max_backoff ? config.max_backoff : current_backoff * 2;
        num_retries++;
    }

    if (result) {
        return result;
    }

    if (stoken.stop_requested()) {
        return UnexpectedError("stop requested");
    }
    return UnexpectedError(std::format("{} reached max retries: {}", config.name, num_retries));
}

template <typename T, typename F>
inline auto ExponentialBackoff(ExponentialConfig config, F &&retryable) -> expected<T, Error> {
    std::stop_token stoken{};
    return ExponentialBackoff<T>(config, std::forward<F>(retryable), stoken);
}

}  // namespace oryx::retry