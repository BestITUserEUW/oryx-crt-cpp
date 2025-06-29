#pragma once

#include <string>
#include <string_view>

namespace oryx {

/**
 * @brief Base error class for exception free programming
 */
class Error {
public:
    explicit Error(std::string what)
        : what_(std::move(what)) {}

    [[nodiscard]] auto what() const -> std::string_view { return what_; }

private:
    std::string what_;
};

};  // namespace oryx