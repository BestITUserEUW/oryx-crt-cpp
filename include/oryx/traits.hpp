#pragma once

#include <type_traits>
#include <functional>

namespace oryx::traits {

template <typename T>
struct is_std_function : std::false_type {};

template <typename R, typename... Args>
struct is_std_function<std::function<R(Args...)>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_function_v = is_std_function<T>::value;

template <typename L>
concept BasicLockable = requires(L m) {
    m.lock();
    m.unlock();
};

}  // namespace oryx::traits