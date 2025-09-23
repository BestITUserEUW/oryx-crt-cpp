#include "doctest.hpp"

#include <oryx/retry.hpp>

using namespace std::chrono_literals;
using namespace oryx;

namespace {
const retry::ExponentialConfig kDefaultConfig{.name = "test", .start_backoff = 0s, .max_backoff = 0s, .max_retries = 1};
}

TEST_CASE("Retryable fails and succeeds") {
    CHECK_FALSE(
        retry::ExponentialBackoff<void>(kDefaultConfig, []() -> void_expected<Error> { return UnexpectedError(""); }));
    CHECK(retry::ExponentialBackoff<void>(kDefaultConfig, []() -> void_expected<Error> { return kVoidExpected; }));
}

TEST_CASE("Retryable succeeds after some tries") {
    int tries = 4;
    int tries_so_far = 0;
    const auto retryable = [&]() -> void_expected<Error> {
        tries_so_far++;

        if (tries_so_far == tries) {
            return kVoidExpected;
        }

        return UnexpectedError("");
    };

    auto config = kDefaultConfig;
    config.max_retries = tries;
    CHECK(retry::ExponentialBackoff<void>(config, retryable));
}

TEST_CASE("Exhausting max retries") {
    int tries = 4;
    int tries_so_far = 0;
    const auto retryable = [&]() -> void_expected<Error> {
        tries_so_far++;

        if (tries_so_far == tries) {
            return kVoidExpected;
        }

        return UnexpectedError("");
    };

    auto config = kDefaultConfig;
    config.max_retries = tries - 1;
    CHECK_FALSE(retry::ExponentialBackoff<void>(config, retryable));
}