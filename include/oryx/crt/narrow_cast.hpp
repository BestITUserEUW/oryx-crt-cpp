#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace oryx::crt {

/**
 * @brief Searchable way to do narrowing casts of values
 */
template <class T, class U>
constexpr auto NarrowCast(U&& u) -> T {
    return static_cast<T>(std::forward<U>(u));
}

/**
 * @brief Checked version of NarrowCast() that returns nullopt if the cast changed the value
 */
template <class T, class U>
constexpr auto Narrow(U u) -> std::optional<T>
    requires std::is_arithmetic_v<T>
{
    constexpr const bool is_different_signedness = (std::is_signed_v<T> != std::is_signed_v<U>);

    const T t = NarrowCast<T>(u);

#if defined(__clang__) || defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
    // Note: NaN will always return nullopt, since NaN != NaN
    if (static_cast<U>(t) != u || (is_different_signedness && ((t < T{}) != (u < U{})))) {
        return std::nullopt;
    }
#if defined(__clang__) || defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

    return t;
}

template <class T, class U>
constexpr auto Narrow(U u) -> std::optional<T>
    requires(!std::is_arithmetic_v<T>)
{
    const T t = NarrowCast<T>(u);
    if (static_cast<U>(t) != u) {
        return std::nullopt;
    }

    return t;
}

}  // namespace oryx::crt