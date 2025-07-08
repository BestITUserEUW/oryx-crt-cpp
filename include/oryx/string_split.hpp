#pragma once

#include <vector>
#include <string_view>

// NOTE: If you are running with C++23 use std::views::split which does not need memory allocation.
// https://en.cppreference.com/w/cpp/ranges/split_view.html

namespace oryx {
namespace detail {
struct StringSplitFunctor {
    constexpr auto operator()(std::string_view input, char delim) const -> std::vector<std::string_view> {
        std::vector<std::string_view> result;
        size_t delim_pos;
        size_t pos{};

        while ((delim_pos = input.find(delim, pos)) != std::string_view::npos) {
            result.emplace_back(input.substr(pos, delim_pos - pos));
            pos = delim_pos + 1;
        }
        if (!result.empty()) result.emplace_back(input.substr(pos));  // Add the last part
        return result;
    }
};
}  // namespace detail

inline constexpr detail::StringSplitFunctor StringSplit{};

}  // namespace oryx