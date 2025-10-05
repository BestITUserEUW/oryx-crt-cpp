#pragma once

#include <memory>
#include <cstdio>
#include <string_view>

namespace oryx::crt {
namespace detail {

struct FileDeleter {
    auto operator()(FILE* file) const { fclose(file); };
};

}  // namespace detail

using UniqueFilePtr = std::unique_ptr<FILE, detail::FileDeleter>;

inline auto OpenFile(std::string_view file_name, std::string_view modes) {
    return UniqueFilePtr{fopen(file_name.data(), modes.data())};
}

}  // namespace oryx::crt