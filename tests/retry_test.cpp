#include "doctest.hpp"

#include <oryx/retry.hpp>
#include <oryx/expected.hpp>
#include <stop_token>

using namespace std::chrono_literals;
using namespace oryx;

namespace {
const retry::ExponentialConfig kDefaultConfig{.start_backoff = 0s, .max_backoff = 0s, .max_retries = 1};
}

TEST_CASE("Retryable fails and succeeds") {
    CHECK_FALSE(
        retry::ExponentialBackoff(kDefaultConfig, []() -> void_expected<Error> { return UnexpectedError(""); }));
    CHECK(retry::ExponentialBackoff(kDefaultConfig, []() -> void_expected<Error> { return kVoidExpected; }));
}

TEST_CASE("Retryable succeeds after some tries") {
    int tries = 4;
    int tries_so_far = 0;
    auto retryable = [&]() -> void_expected<Error> {
        tries_so_far++;

        if (tries_so_far == tries) {
            return kVoidExpected;
        }

        return UnexpectedError("");
    };

    auto config = kDefaultConfig;
    config.max_retries = tries;
    CHECK(retry::ExponentialBackoff(config, retryable));
}

TEST_CASE("Exhausting max retries") {
    int tries = 4;
    int tries_so_far = 0;
    auto retryable = [&]() -> void_expected<Error> {
        tries_so_far++;

        if (tries_so_far == tries) {
            return kVoidExpected;
        }

        return UnexpectedError("");
    };

    auto config = kDefaultConfig;
    config.max_retries = tries - 1;
    CHECK_FALSE(retry::ExponentialBackoff(config, retryable));
}

TEST_CASE("Passing a custom predicate") {
    bool flag{};
    int tries_so_far = 0;
    int tries = 5;

    auto never_succeeds = [&] -> bool {
        if (++tries_so_far == tries) {
            flag = true;
        }
        return false;
    };

    auto stopper = [&flag] -> bool { return flag; };
    auto config = kDefaultConfig;
    config.max_retries = 10;
    CHECK_FALSE(retry::ExponentialBackoff(config, never_succeeds, stopper));
    CHECK_EQ(tries_so_far, 5);
}

TEST_CASE("Works with stop token as expected") {
    std::stop_source ssource{};
    int tries_so_far = 0;
    int tries = 5;

    auto never_succeeds = [&] -> bool {
        if (++tries_so_far == tries) {
            ssource.request_stop();
        }
        return false;
    };
    auto config = kDefaultConfig;
    config.max_retries = 10;
    CHECK_FALSE(retry::ExponentialBackoff(config, never_succeeds, ssource.get_token()));
    CHECK_EQ(tries_so_far, 5);
}