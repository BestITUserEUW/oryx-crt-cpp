#pragma once

#include <utility>

#define ORYX_DEFINE_ENUM_CLASS_BITWISE_OPERATORS(EnumType)                                               \
    inline constexpr auto operator|(EnumType lhs, EnumType rhs)->EnumType {                              \
        return static_cast<EnumType>(std::to_underlying(lhs) | std::to_underlying(rhs));                 \
    }                                                                                                    \
    inline constexpr auto operator&(EnumType lhs, EnumType rhs)->EnumType {                              \
        return static_cast<EnumType>(std::to_underlying(lhs) & std::to_underlying(rhs));                 \
    }                                                                                                    \
    inline constexpr auto operator^(EnumType lhs, EnumType rhs)->EnumType {                              \
        return static_cast<EnumType>(std::to_underlying(lhs) ^ std::to_underlying(rhs));                 \
    }                                                                                                    \
    inline constexpr auto operator~(EnumType val)->EnumType {                                            \
        return static_cast<EnumType>(~std::to_underlying(val));                                          \
    }                                                                                                    \
    inline constexpr auto operator|=(EnumType& lhs, EnumType rhs)->EnumType& { return lhs = lhs | rhs; } \
    inline constexpr auto operator&=(EnumType& lhs, EnumType rhs)->EnumType& { return lhs = lhs & rhs; } \
    inline constexpr auto operator^=(EnumType& lhs, EnumType rhs)->EnumType& { return lhs = lhs ^ rhs; }
