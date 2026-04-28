// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#pragma once

#include <numbers>

namespace Shared {
    constexpr float pi = 3.1415927f;
    constexpr float half_pi = pi / 2.0f;

    constexpr auto deg2rad(const float deg) -> float {
        return deg * pi / 180.0f;
    }

    constexpr auto rad2deg(const float rad) -> float {
        return rad * 180.0f / pi;
    }
}