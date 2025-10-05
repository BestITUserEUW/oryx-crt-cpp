#pragma once

#include <string_view>
#include <optional>
#include <cstdint>
#include <charconv>

// std::optional wrapper around the from chars interface

namespace oryx::crt {

/**
 * @brief std::from_chars wrapper, returning nullopt if from chars reports error
 *
 * @tparam T
 * @param s
 * @return std::optional<T>
 */
template <typename T>
constexpr auto FromChars(std::string_view s) -> std::optional<T> {
    T val;
    if (std::from_chars(s.data(), s.data() + s.size(), val).ec == std::errc{}) {
        return val;
    } else {
        return std::nullopt;
    }
}

template <>
constexpr auto FromChars<bool>(std::string_view s) -> std::optional<bool> {
#if __cpp_lib_optional >= 202110L
    return FromChars<uint8_t>(s).transform([](uint8_t val) { return static_cast<bool>(val); });
#else
    const auto result = FromChars<uint8_t>(s);
    if (result)
        return static_cast<bool>(*result);
    else
        return std::nullopt;
#endif
}

}  // namespace oryx::crt