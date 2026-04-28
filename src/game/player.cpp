// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#include "player.hpp"

#include <cmath>

#include "game/shared.hpp"

Player::Player(const Level& level) : level_(level) {
    x_ = static_cast<float>((level_.Width() - Player::size_) >> 1);
    y_ = static_cast<float>((level_.Height() - Player::size_) >> 1);
    dir_ = Shared::deg2rad(90.0f);
}

auto Player::DrawMinimap(Pixels& pixels) const -> void {
    pixels.SetFill({.r = 255, .g = 0, .b = 0});
    pixels.NoStroke();
    pixels.Rect(x_, y_, Player::size_, Player::size_);

    pixels.SetStroke({.r = 0, .g = 0, .b = 255});

    const auto offset = Player::size_ / 2;
    pixels.Line(
        x_ + offset, y_ + offset,
        x_ + offset + std::cos(dir_) * 50,
        y_ + offset + std::sin(dir_) * 50
    );
}