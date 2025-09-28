#pragma once

#include <string>
#include <optional>
#include <algorithm>
#include <functional>

#include "from_chars.hpp"

namespace oryx::argparse {
namespace details {
template <typename>
constexpr bool always_false = false;
}

class CLI {
public:
    CLI(int argc, const char* const* argv)
        : view_(argv, argc) {}

    auto Contains(std::string_view option) const { return std::ranges::find(view_, option) != view_.end(); }

    template <class T>
    [[nodiscard]] constexpr auto GetValue(std::string_view option) const -> std::optional<T> {
        auto it = std::ranges::find(view_, option);
        if (it == view_.end()) {
            return std::nullopt;
        }

        auto vit = it + 1;
        if (vit == view_.end()) {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<T, std::string>) {
            return std::string(*vit);
        } else if constexpr (std::is_integral_v<T>) {
            return FromChars<T>(*vit);
        } else if constexpr (std::is_floating_point_v<T>) {
            return FromChars<T>(*vit);
        } else {
            static_assert(details::always_false<T>,
                          "Get argument only supports std::string integral and floating point types");
        }
    }

    template <class T>
    void VisitIfContains(std::string_view option, auto visitor) {
        auto value = GetValue<T>(option);
        if (value) {
            std::invoke(visitor, std::forward<T>(*value));
        }
    }

    auto Empty() const { return view_.empty(); }
    auto Size() const { return view_.size(); }

private:
    std::basic_string_view<const char*> view_;
};

}  // namespace oryx::argparse