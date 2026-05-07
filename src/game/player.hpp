#pragma once

#include "core/pixels.hpp"

#include "game/level.hpp"

// =============================================================================
// Player — first-person agent in “world pixel” space
// =============================================================================
//
// State:
//   (x_, y_)     — top-left corner of a size_×size_ square hitbox in world pixels.
//   dir_         — heading in radians; forward = (cos(dir_), sin(dir_)), same as main’s ray dir.
//
// Why world pixels:
//   Level::TILE_SIZE converts between grid and world; raycaster and minimap both use the same space.
//
// Movement model (see player.cpp):
//   • Turn: add to dir_, wrap to [0, 2π).
//   • Move: try to step the *center* along the look vector; if blocked, try X-only then Y-only
//     (axis slide) so the player can scrape along walls.
//   • Collision: four corner samples of a small box (collision_radius_) — not a full sweep test.
//
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
