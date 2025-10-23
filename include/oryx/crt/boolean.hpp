#pragma once

#include <functional>
#include <utility>

namespace oryx::crt {

class Boolean {
public:
    constexpr Boolean() = default;

    constexpr Boolean(bool value)
        : value_(value) {}

    operator bool() const { return value_; }

    template <typename F>
        requires std::invocable<F>
    constexpr auto AndThen(F&& f) const -> Boolean {
        if (value_) {
            return std::invoke(std::forward<F>(f));
        }

        return *this;
    }

    template <typename F>
        requires std::invocable<F>
    constexpr auto OrElse(F&& f) const -> Boolean {
        if (!value_) {
            return std::invoke(std::forward<F>(f));
        }

        return *this;
    }

    template <typename F>
        requires std::invocable<F, bool>
    constexpr auto Transform(F&& f) const {
        return std::invoke(std::forward<F>(f), value_);
    }

    constexpr auto operator==(const Boolean& other) const -> bool { return value_ == other.value_; }
    constexpr auto operator!=(const Boolean& other) const -> bool { return value_ != other.value_; }
    constexpr auto operator&&(const Boolean& other) const -> Boolean { return value_ && other.value_; }
    constexpr auto operator||(const Boolean& other) const -> Boolean { return value_ || other.value_; }
    constexpr auto operator!() const -> Boolean { return !value_; }

    constexpr auto value() const -> bool { return value_; }

private:
    bool value_{};
};

}  // namespace oryx::crt