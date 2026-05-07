#pragma once

#include <numbers>

// Small math helpers shared by gameplay (angles use radians internally).
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
