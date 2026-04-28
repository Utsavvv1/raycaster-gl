// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#include "plane.hpp"

Plane::Plane(
    float width,
    float height,
    unsigned width_segments,
    unsigned height_segments
) :
  width(width),
  height(height),
  width_segments(width_segments),
  height_segments(height_segments)
{
    auto width_half = width / 2;
    auto height_half = height / 2;

    auto grid_x = width_segments;
    auto grid_y = height_segments;
    auto grid_x1 = grid_x + 1;
    auto grid_y1 = grid_y + 1;

    auto segment_w = width / grid_x;
    auto segment_h = height / grid_y;

    for (auto iy = 0; iy < grid_y1; ++iy) {
        const auto y = iy * segment_h - height_half;
        for (auto ix = 0; ix < grid_x1; ++ix) {
            const auto x = ix * segment_w - width_half;
            const auto u = static_cast<float>(ix) / grid_x;
            const auto v = 1 - (static_cast<float>(iy) / grid_y);

            vertices_.emplace_back(x);  // pos x
            vertices_.emplace_back(-y); // pos y
            vertices_.emplace_back(0);  // pos z
            vertices_.emplace_back(0);  // normal x
            vertices_.emplace_back(0);  // normal y
            vertices_.emplace_back(1);  // normal z
            vertices_.emplace_back(u);  // u
            vertices_.emplace_back(v);  // v
        }
    }

    for (auto iy = 0; iy < grid_y; ++iy) {
        for (auto ix = 0; ix < grid_x; ++ix) {
            const auto a = ix + grid_x1 * iy;
            const auto b = ix + grid_x1 * (iy + 1);
            const auto c = ix + 1 + grid_x1 * (iy + 1);
            const auto d = ix + 1 + grid_x1 * iy;

            indices_.emplace_back(a);
            indices_.emplace_back(b);
            indices_.emplace_back(d);
            indices_.emplace_back(b);
            indices_.emplace_back(c);
            indices_.emplace_back(d);
        }
    }
}