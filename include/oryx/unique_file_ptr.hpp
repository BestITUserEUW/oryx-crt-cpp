#pragma once

#include <memory>
#include <cstdio>
#include <string_view>

namespace oryx {

using UniqueFilePtr = std::unique_ptr<FILE, decltype([](FILE* file) { fclose(file); })>;

inline auto OpenFile(std::string_view file_name, std::string_view modes) {
    return UniqueFilePtr{fopen(file_name.data(), modes.data())};
}

}  // namespace oryx