#pragma once

#include <vector>
#include <format>

#include "error.hpp"

namespace oryx::crt {

class ErrorGroup {
public:
    ErrorGroup() = default;

    ErrorGroup(std::initializer_list<Error> errors)
        : errors_(errors) {}

    void Add(Error err) { errors_.push_back(std::move(err)); }

    [[nodiscard]] auto Size() const noexcept -> size_t { return errors_.size(); }

    [[nodiscard]] auto Empty() const noexcept -> bool { return errors_.empty(); }

    [[nodiscard]] auto Errors() const -> const std::vector<Error>& { return errors_; }

    [[nodiscard]] auto ToError() const -> Error { return Error(what()); }

    [[nodiscard]] auto what() const -> std::string {
        auto what = std::format("ErrorGroup with {} error(s):\n", errors_.size());
        for (size_t i = 0; i < errors_.size(); ++i) {
            what.append(std::format("[{}] {}", i, errors_[i].what()));
        }
        return what;
    }

private:
    std::vector<Error> errors_;
};

}  // namespace oryx::crt