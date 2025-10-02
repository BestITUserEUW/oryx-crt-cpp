#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <cassert>

namespace oryx {

template <class T>
class LazyComponent {
public:
    using FactoryContainer = std::move_only_function<T()>;

    LazyComponent() = default;

    LazyComponent()
        requires std::is_default_constructible_v<T>
        : factory_([] { return T(); }) {}

    // Constructor that takes a creator callable
    template <typename Factory>
        requires std::invocable<Factory> && std::same_as<std::invoke_result_t<Factory>, T>
    explicit LazyComponent(Factory&& factory)
        : factory_(std::forward<Factory>(factory)) {}

    auto operator*() -> T& { return *Get(); }
    auto operator*() const -> const T& { return *Get(); }
    auto operator->() -> T* { return Get(); }
    auto operator->() const -> const T* { return Get(); }

    void Init() { InitOnce(); }

    auto Get() -> T* {
        InitOnce();
        return value_.get();
    }

    auto Get() const -> const T* {
        InitOnce();
        return value_.get();
    }

    auto IsInitialized() const noexcept -> bool { return value_ != nullptr; }

private:
    void InitOnce() {
        assert(static_cast<bool>(factory_) && "LazyComponent requires valid factory callable!");
        std::call_once(created_flag_, [this] { value_ = std::make_unique<T>(std::invoke(factory_)); });
    }

    std::once_flag created_flag_{};
    std::move_only_function<T()> factory_{};
    mutable std::unique_ptr<T> value_{};
};

template <class Factory>
LazyComponent(Factory) -> LazyComponent<std::invoke_result_t<Factory>>;

template <class Factory>
auto MakeLazy(Factory&& factory) {
    return LazyComponent<std::invoke_result_t<Factory>>(std::forward<Factory>(factory));
}

}  // namespace oryx