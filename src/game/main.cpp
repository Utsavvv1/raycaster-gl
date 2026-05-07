#include "core/pixels.hpp"

#include <algorithm>
#include <cmath>

#include "game/level.hpp"
#include "game/player.hpp"
#include "game/shared.hpp"

namespace {
constexpr auto kFov = Shared::deg2rad(60.0f);

auto DrawBackground(Pixels& pixels) -> void {
    const auto width = pixels.Width();
    const auto height = pixels.Height();
    const auto half_height = height / 2;

    pixels.FillRect(0, 0, width, half_height, {.r = 25, .g = 25, .b = 55});
    pixels.FillRect(0, half_height, width, height - half_height, {.r = 40, .g = 30, .b = 20});
}

auto DrawRaycastScene(const Level& level, const Player& player, Pixels& pixels) -> void {
    const auto screen_width = static_cast<int>(pixels.Width());
    const auto screen_height = static_cast<int>(pixels.Height());
    const auto camera_x = player.CenterX() / static_cast<float>(Level::TILE_SIZE);
    const auto camera_y = player.CenterY() / static_cast<float>(Level::TILE_SIZE);
    const auto dir = player.Direction();

    const auto dir_x = std::cos(dir);
    const auto dir_y = std::sin(dir);
    const auto plane_scale = std::tan(kFov * 0.5f);
    const auto plane_x = -dir_y * plane_scale;
    const auto plane_y = dir_x * plane_scale;

    for (auto x = 0; x < screen_width; ++x) {
        const auto ray_camera = 2.0f * static_cast<float>(x) / static_cast<float>(screen_width) - 1.0f;
        const auto ray_dir_x = dir_x + plane_x * ray_camera;
        const auto ray_dir_y = dir_y + plane_y * ray_camera;

        auto map_x = static_cast<int>(std::floor(camera_x));
        auto map_y = static_cast<int>(std::floor(camera_y));

        const auto delta_dist_x = (ray_dir_x == 0.0f) ? 1e30f : std::abs(1.0f / ray_dir_x);
        const auto delta_dist_y = (ray_dir_y == 0.0f) ? 1e30f : std::abs(1.0f / ray_dir_y);

        auto step_x = 0;
        auto step_y = 0;
        auto side_dist_x = 0.0f;
        auto side_dist_y = 0.0f;

        if (ray_dir_x < 0.0f) {
            step_x = -1;
            side_dist_x = (camera_x - static_cast<float>(map_x)) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (static_cast<float>(map_x + 1) - camera_x) * delta_dist_x;
        }
        if (ray_dir_y < 0.0f) {
            step_y = -1;
            side_dist_y = (camera_y - static_cast<float>(map_y)) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (static_cast<float>(map_y + 1) - camera_y) * delta_dist_y;
        }

        auto hit = false;
        auto side = 0; // 0 = x-side, 1 = y-side.
        constexpr auto kMaxSteps = 128;
        for (auto steps = 0; steps < kMaxSteps; ++steps) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }

            if (level.IsWallTile(map_x, map_y)) {
                hit = true;
                break;
            }
        }

        if (!hit) {
            continue;
        }

        auto perp_wall_dist = 0.0f;
        if (side == 0) {
            perp_wall_dist = (static_cast<float>(map_x) - camera_x + (1.0f - static_cast<float>(step_x)) * 0.5f) / ray_dir_x;
        } else {
            perp_wall_dist = (static_cast<float>(map_y) - camera_y + (1.0f - static_cast<float>(step_y)) * 0.5f) / ray_dir_y;
        }
        if (perp_wall_dist <= 0.0f) {
            continue;
        }

        const auto line_height = static_cast<int>(static_cast<float>(screen_height) / perp_wall_dist);
        const auto draw_start = std::max(0, (screen_height - line_height) / 2);
        const auto draw_end = std::min(screen_height - 1, (screen_height + line_height) / 2);

        auto wall_color = RGB {.r = 190, .g = 190, .b = 190};
        if (side == 1) {
            wall_color = RGB {.r = 140, .g = 140, .b = 140};
        }
        pixels.VLine(x, draw_start, draw_end, wall_color);
    }
}
} // namespace

auto main() -> int {
    auto level = Level {};
    auto player = Player { level };
    auto pixels = Pixels { level.Width(), level.Height(), "Raycaster OpenGL" };

    pixels.Update([&](const double delta) {
        const auto move_forward = pixels.IsKeyDown(GLFW_KEY_W) || pixels.IsKeyDown(GLFW_KEY_UP);
        const auto move_backward = pixels.IsKeyDown(GLFW_KEY_S) || pixels.IsKeyDown(GLFW_KEY_DOWN);
        const auto turn_left = pixels.IsKeyDown(GLFW_KEY_A) || pixels.IsKeyDown(GLFW_KEY_LEFT);
        const auto turn_right = pixels.IsKeyDown(GLFW_KEY_D) || pixels.IsKeyDown(GLFW_KEY_RIGHT);

        player.Update(delta, move_forward, move_backward, turn_left, turn_right);
    });

    pixels.Draw([&]() {
        DrawBackground(pixels);
        DrawRaycastScene(level, player, pixels);

        constexpr auto minimap_tile_size = 8u;
        constexpr auto minimap_offset_x = 16u;
        constexpr auto minimap_offset_y = 16u;
        level.DrawMinimap(pixels, minimap_tile_size, minimap_offset_x, minimap_offset_y);
        player.DrawMinimap(pixels, minimap_tile_size, minimap_offset_x, minimap_offset_y);
    });

    pixels.Run();

    return 0;
}