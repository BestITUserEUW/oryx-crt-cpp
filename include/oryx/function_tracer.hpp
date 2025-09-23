#pragma once

#if __has_include(<source_location>) && __has_include(<print>)

    #include <print>
    #include <source_location>
    #include <string_view>

    #include <oryx/chrono/stopwatch.hpp>

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

template <class Stopwatch = chrono::Stopwatch>
class FunctionTimingTracer {
public:
    constexpr FunctionTimingTracer(const std::source_location loc = std::source_location::current())
        : name_(loc.function_name()),
          sw_() {}
    ~FunctionTimingTracer() {
        auto elapsed = sw_.ElapsedMs();
        std::println("Function {} took {}", name_, elapsed);
    }

private:
    std::string_view name_;
    Stopwatch sw_;
};

}  // namespace oryx

    #define ORYX_TRACE_FUNCTION() oryx::FunctionTracer __oryx__func_tracer__
    #define ORYX_TRACE_FUNCTION_TIMING() oryx::FunctionTimingTracer __oryx__func_timing_tracer__
#else
    #define ORYX_TRACE_FUNCTION() (void)0
    #define ORYX_TRACE_FUNCTION_TIMING() (void)0
#endif