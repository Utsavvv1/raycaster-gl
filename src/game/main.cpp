// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#include "core/pixels.hpp"

#include "game/level.hpp"
#include "game/player.hpp"

auto main() -> int {
    auto level = Level {};
    auto player = Player { level };
    auto pixels = Pixels { level.Width(), level.Height(), "Raycaster OpenGL" };

    pixels.Update([&](const double delta) {
        // TODO: impl.
    });

    pixels.Draw([&]() {
        level.DrawMinimap(pixels);
        player.DrawMinimap(pixels);
    });

    pixels.Run();

    return 0;
}