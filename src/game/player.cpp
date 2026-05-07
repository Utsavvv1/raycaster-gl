// player.cpp — implements Player (see player.hpp for coordinate + collision overview).

#include "player.hpp"

#include <algorithm>
#include <cmath>

#include "game/shared.hpp"

// Spawn in the map center, facing “up” on screen (90° from +X in standard math = toward +Y world).
Player::Player(const Level& level) : level_(level) {
    x_ = static_cast<float>((level_.Width() - Player::size_) >> 1);
    y_ = static_cast<float>((level_.Height() - Player::size_) >> 1);
    dir_ = Shared::deg2rad(90.0f);
}

namespace {

// Keep heading in [0, 2π) so turning forever doesn’t drift float error into huge angles.
auto NormalizeAngle(float angle) -> float {
    constexpr auto two_pi = Shared::pi * 2.0f;
    angle = std::fmod(angle, two_pi);
    if (angle < 0.0f) {
        angle += two_pi;
    }

    return angle;
}
} // namespace

auto Player::Update(double delta, bool move_forward, bool move_backward, bool turn_left, bool turn_right) -> void {
    // --- Rotation (radians / second scaled by input -1, 0, or +1) ---
    auto turn_axis = 0.0f;
    if (turn_left) {
        turn_axis -= 1.0f;
    }
    if (turn_right) {
        turn_axis += 1.0f;
    }
    dir_ = NormalizeAngle(dir_ + turn_axis * rotate_speed_ * static_cast<float>(delta));

    // --- Translation: intent along look vector (may slide along walls; see below) ---
    auto move_axis = 0.0f;
    if (move_forward) {
        move_axis += 1.0f;
    }
    if (move_backward) {
        move_axis -= 1.0f;
    }

    if (move_axis == 0.0f) {
        return;
    }

    auto center_x = CenterX();
    auto center_y = CenterY();

    const auto move_step = move_axis * move_speed_ * static_cast<float>(delta);
    const auto next_x = center_x + std::cos(dir_) * move_step;
    const auto next_y = center_y + std::sin(dir_) * move_step;

    // Collision = axis-aligned bounding box (AABB) vs tile walls: four corner probes.
    // Not a swept capsule — fast and good enough for grid worlds.
    const auto can_move = [&](float world_x, float world_y) -> bool {
        const auto left = world_x - collision_radius_;
        const auto right = world_x + collision_radius_;
        const auto top = world_y - collision_radius_;
        const auto bottom = world_y + collision_radius_;

        return !level_.IsWallAtWorld(left, top)
            && !level_.IsWallAtWorld(right, top)
            && !level_.IsWallAtWorld(left, bottom)
            && !level_.IsWallAtWorld(right, bottom);
    };

    // Try X motion alone, then Y alone → diagonal moves slide along corridors instead of stopping dead.
    if (can_move(next_x, center_y)) {
        center_x = next_x;
    }
    if (can_move(center_x, next_y)) {
        center_y = next_y;
    }

    constexpr auto half_size = static_cast<float>(size_) * 0.5f;
    x_ = center_x - half_size;
    y_ = center_y - half_size;
}

auto Player::DrawMinimap(Pixels& pixels) const -> void {
    DrawMinimap(pixels, Level::TILE_SIZE, 0, 0);
}

// Scale world position to minimap pixels: tile_size/TILE_SIZE is the zoom factor.
auto Player::DrawMinimap(Pixels& pixels, unsigned tile_size, unsigned offset_x, unsigned offset_y) const -> void {
    const auto scale = static_cast<float>(tile_size) / static_cast<float>(Level::TILE_SIZE);
    const auto marker_size = std::max(2u, static_cast<unsigned>(std::round(size_ * scale)));
    const auto px = static_cast<unsigned>(offset_x + x_ * scale);
    const auto py = static_cast<unsigned>(offset_y + y_ * scale);

    pixels.SetFill({.r = 255, .g = 0, .b = 0});
    pixels.NoStroke();
    pixels.Rect(px, py, marker_size, marker_size);

    pixels.SetStroke({.r = 0, .g = 0, .b = 255});

    const auto offset = static_cast<float>(marker_size) * 0.5f;
    const auto ray_len = std::max(10.0f, 50.0f * scale);
    pixels.Line(
        static_cast<unsigned>(px + offset),
        static_cast<unsigned>(py + offset),
        static_cast<unsigned>(px + offset + std::cos(dir_) * ray_len),
        static_cast<unsigned>(py + offset + std::sin(dir_) * ray_len)
    );
}
