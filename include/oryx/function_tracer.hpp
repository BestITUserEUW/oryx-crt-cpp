#pragma once

#if __has_include(<source_location>) && __has_include(<print>)

    #include <print>
    #include <source_location>
    #include <string_view>

namespace oryx {

class FunctionTracer {
public:
    constexpr FunctionTracer(const std::source_location loc = std::source_location::current())
        : name_(loc.function_name()) {
        std::println("Entering {}", name_);
    }
    ~FunctionTracer() { std::println("Exiting {}", name_); }

private:
    std::string_view name_;
};

}  // namespace oryx

    #define ORYX_TRACE_FUNCTION() oryx::FunctionTracer __func_tracer__
#else
    #define ORYX_TRACE_FUNCTION() (void)0
#endif