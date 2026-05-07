#pragma once

#include <chrono>

#include <fmt/printf.h>

using Clock = std::chrono::steady_clock;

// =============================================================================
// Timer — minimal stopwatch for frame deltas
// =============================================================================
//
// Usage pattern (see Window::Start):
//   Reset() once before the loop;
//   each iteration: dt = GetSeconds(); Reset(); → dt is elapsed wall time since last Reset.
//
// steady_clock = monotonic (not affected by system clock adjustments); good for animation dt.
//
class Timer {
public:
    Timer() : start_time_(Clock::now()) {}

    auto GetMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now() - start_time_
        ).count();
    }

    auto GetSeconds() const {
        return std::chrono::duration_cast<std::chrono::duration<double>>(
            Clock::now() - start_time_
        ).count();
    }

    auto Reset() { start_time_ = Clock::now(); }

    auto Print() const { fmt::print("{}ms\n", GetMilliseconds()); }

private:
    std::chrono::time_point<Clock> start_time_;
};
