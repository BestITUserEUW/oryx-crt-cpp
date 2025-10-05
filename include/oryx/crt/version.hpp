#pragma once

#include <string>

namespace oryx::crt {

inline constexpr int kVersionMajor = 0;
inline constexpr int KVersionMinor = 3;
inline constexpr int kVersionPatch = 0;

auto MakeStringVersion() -> std::string;

}  // namespace oryx::crt