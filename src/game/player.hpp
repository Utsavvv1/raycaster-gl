// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#pragma once

#include "core/pixels.hpp"

#include "game/level.hpp"

class Player {
public:
    explicit Player(const Level& level);

    auto DrawMinimap(Pixels& pixels) const -> void;

private:
    static constexpr unsigned size_ = 10;

    float x_ = 0.0f;
    float y_ = 0.0f;
    float dir_ = 0.0f;

    Level level_;
};