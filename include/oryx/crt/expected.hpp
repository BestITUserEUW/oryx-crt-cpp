#pragma once

#include <string>
#include <expected>

#include "error.hpp"

#ifdef __cpp_lib_expected

namespace oryx::crt {

template <class T, class E>
using expected = std::expected<T, E>;

template <class E>
using unexpected = std::unexpected<E>;

template <class E>
using void_expected = expected<void, E>;

inline constexpr void_expected<Error> kVoidExpected{};

[[nodiscard]] inline auto UnexpectedError(std::string&& what) {
    return unexpected<Error>(std::forward<std::string>(what));
}
[[nodiscard]] inline auto UnexpectedError(const char* what) { return unexpected<Error>(what); }
[[nodiscard]] inline auto UnexpectedError(const std::string& what) { return unexpected<Error>(what); }
[[nodiscard]] inline auto UnexpectedError(const std::exception& exc) { return unexpected<Error>(exc.what()); }

#endif

}  // namespace oryx