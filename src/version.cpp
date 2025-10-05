#include <oryx/crt/version.hpp>

#include <format>

namespace oryx::crt {

auto MakeStringVersion() -> std::string { return std::format("{}.{}.{}", kVersionMajor, KVersionMinor, kVersionPatch); }

}  // namespace oryx::crt