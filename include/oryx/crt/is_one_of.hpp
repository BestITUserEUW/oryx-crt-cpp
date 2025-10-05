#pragma once

namespace oryx::crt {

template <typename T, typename... Args>
inline constexpr auto IsOneOf(const T& value, const Args&... args) -> bool {
    return ((value == args) || ...);
}

}  // namespace oryx::crt