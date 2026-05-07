#pragma once

#include "core/pixels.hpp"

#include "game/level.hpp"

class Player {
public:
    explicit Player(const Level& level);
    auto Update(double delta, bool move_forward, bool move_backward, bool turn_left, bool turn_right) -> void;

    auto DrawMinimap(Pixels& pixels) const -> void;
    auto DrawMinimap(Pixels& pixels, unsigned tile_size, unsigned offset_x, unsigned offset_y) const -> void;
    auto X() const { return x_; }
    auto Y() const { return y_; }
    auto Direction() const { return dir_; }
    auto CenterX() const { return x_ + static_cast<float>(size_) * 0.5f; }
    auto CenterY() const { return y_ + static_cast<float>(size_) * 0.5f; }

private:
    static constexpr unsigned size_ = 10;
    static constexpr float move_speed_ = 190.0f;
    static constexpr float rotate_speed_ = 2.3f;
    static constexpr float collision_radius_ = 4.0f;

    float x_ = 0.0f;
    float y_ = 0.0f;
    float dir_ = 0.0f;

    const Level& level_;
};