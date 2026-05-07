#pragma once

#include <numbers>

// =============================================================================
// Shared — angle helpers for gameplay + raycaster (everything uses radians)
// =============================================================================
//
// Convention:
//   • Internal math uses radians only (Player::dir_, main.cpp trig, etc.).
//   • Use deg2rad() when entering from “human” units (e.g. FOV 60°, spawn facing 90°).
//
// Forward vector from heading `t`:
//   (cos(t), sin(t)) — matches standard math angles (0 rad → +X, increases CCW).
//
namespace Shared {
    constexpr float pi = std::numbers::pi_v<float>;
    constexpr float half_pi = pi / 2.0f;

    constexpr auto deg2rad(const float deg) -> float {
        return deg * pi / 180.0f;
    }

    constexpr auto rad2deg(const float rad) -> float {
        return rad * 180.0f / pi;
    }
}
