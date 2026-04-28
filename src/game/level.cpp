// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#include "level.hpp"

auto Level::DrawMinimap(Pixels& pixels) const -> void {
    pixels.SetStroke({0, 0, 0});
    for (auto i = 0; i < Rows(); ++i) {
        for (auto j = 0; j < Cols(); ++j) {
            const auto x = j * Level::TILE_SIZE;
            const auto y = i * Level::TILE_SIZE;
            if (data_[i][j] == 1) {
                pixels.SetFill({0, 0, 0});
            } else {
                pixels.SetFill({255, 255, 255});
            }
            pixels.Rect(x, y, TILE_SIZE, TILE_SIZE);
        }
    }
}