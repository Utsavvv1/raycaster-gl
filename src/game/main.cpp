// =============================================================================
// main.cpp — Wolfenstein-style 2.5D raycaster (CPU) + OpenGL presentation (GPU)
// =============================================================================
//
// BIG PICTURE (one frame):
//   1. Window clears the real framebuffer (black).
//   2. Pixels::Run asks us for: Update(delta) then Draw().
//   3. Update: read keyboard, move/rotate the Player in world pixel space.
//   4. Draw: fill the *software* RGB buffer — sky, floor, vertical wall strips,
//      then a small minimap overlay.
//   5. Pixels uploads that buffer to a GL texture and draws one fullscreen quad.
//
// COORDINATE SYSTEMS YOU MUST KEEP STRAIGHT:
//   • World pixels: Player and Level use this (origin top-left of the map bitmap).
//   • Tile / map space: divide world x,y by TILE_SIZE → which grid cell we’re in.
//   • Raycasting uses map space for DDA; Player stores center in world pixels.
//
// WHY COLUMN-BASED RAYS:
//   Classic raycasting casts ONE ray per screen column. Each ray hits the nearest
//   wall; distance → how tall to draw that column (fake perspective).

#include "core/pixels.hpp"

#include <algorithm>
#include <cmath>

#include "game/level.hpp"
#include "game/player.hpp"
#include "game/shared.hpp"

// TL;DR: Update() = keys→player; Draw() = sky/floor + raycast columns + minimap; Run() uploads buffer & swaps frames.

namespace {

// How wide the view feels horizontally (radians). Bigger = see more wall at once at the edges.
constexpr auto kFov = Shared::deg2rad(60.0f);

// -----------------------------------------------------------------------------
// Sky band + ground band (no textures yet — solid colors).
// -----------------------------------------------------------------------------
auto DrawBackground(Pixels& pixels) -> void {
    const auto width = pixels.Width();
    const auto height = pixels.Height();
    const auto half_height = height / 2;

    // Cheap filler — walls get drawn on top of this in DrawRaycastScene.
    pixels.FillRect(0, 0, width, half_height, {.r = 25, .g = 25, .b = 55}); // top half ≈ “sky”
    pixels.FillRect(0, half_height, width, height - half_height, {.r = 40, .g = 30, .b = 20}); // bottom ≈ “floor”
}

// -----------------------------------------------------------------------------
// DrawRaycastScene — per-column DDA raycast into the pixel buffer
// -----------------------------------------------------------------------------
//
// Algorithm sketch (repeat for x = 0 .. screen_width-1):
//
//   A) BUILD RAY DIRECTION for this column
//      • Camera faces `dir` (radians). dir_x/dir_y is a unit vector on the map plane.
//      • “Camera plane” is perpendicular to dir; its half-length encodes horizontal FOV.
//      • Screen column x maps to a blend factor ray_camera in [-1, 1].
//      • Actual ray direction = dir + plane * ray_camera  → sweeps left-to-right across view.
//
//   B) DDA — walk the square grid until we hit a wall tile
//      • delta_dist_* = distance along ray to cross one vertical / horizontal grid line.
//      • side_dist_*  = distance from camera to the *next* crossing on each axis.
//      • Each step: cross whichever boundary is closer; record whether we stepped in X or Y
//        (`side`) for shading.
//      • 1e30 hack: if ray is parallel to an axis, avoid divide-by-zero; that axis never wins.
//
//   C) DISTANCE TO WALL — must use “perpendicular” distance, not Euclidean along the ray,
//      or walls bow outward (fisheye). Formula matches which side we hit (side 0 vs 1).
//
//   D) COLUMN HEIGHT — farther → shorter strip; center strip vertically on screen.
//
//   E) SHADE — same wall gets two tones: NS walls vs EW walls (side 0 vs 1).

auto DrawRaycastScene(const Level& level, const Player& player, Pixels& pixels) -> void {
    // Same WxH as the Pixels buffer / texture — one ray per pixel column here.
    const auto screen_width = static_cast<int>(pixels.Width());
    const auto screen_height = static_cast<int>(pixels.Height());

    // Player lives in pixels elsewhere; here we want “which tile am I in?” math → divide by tile size.
    // Camera in tile space — fractions OK (say 5.2, 8.7 = inside cell column 5, row 8).
    const auto camera_x = player.CenterX() / static_cast<float>(Level::TILE_SIZE);
    const auto camera_y = player.CenterY() / static_cast<float>(Level::TILE_SIZE);
    const auto dir = player.Direction();

    // Facing on the map (angle 0 → +X, CCW) — basically the unit “where we look” vector.
    const auto dir_x = std::cos(dir);
    const auto dir_y = std::sin(dir);

    // Camera “plane”: 90° to dir, length tan(half-FOV). dir ± plane sweeps the whole horizontal view.
    const auto plane_scale = std::tan(kFov * 0.5f);
    const auto plane_x = -dir_y * plane_scale; // to the “left” of facing on the map
    const auto plane_y = dir_x * plane_scale;

    // OUTER: sweep left→right. INNER (below): shoot one ray, draw one vertical wall slab for this x.
    for (auto x = 0; x < screen_width; ++x) {
        // Which slice of the view this column is: left edge of screen ≈ -1, right ≈ +1, middle ≈ 0.
        const auto ray_camera = 2.0f * static_cast<float>(x) / static_cast<float>(screen_width) - 1.0f;
        // One cast direction per column = forward + sideways; doesn’t have to be unit length for DDA.
        const auto ray_dir_x = dir_x + plane_x * ray_camera;
        const auto ray_dir_y = dir_y + plane_y * ray_camera;

        // Grid tile under our feet (floor = tile index).
        auto map_x = static_cast<int>(std::floor(camera_x));
        auto map_y = static_cast<int>(std::floor(camera_y));

        // delta = ray-length to hop exactly one grid line on that axis (think “price per grout line”).
        // 1e30 = “never pick me first” when the ray is parallel to that axis (no divide-by-zero).
        const auto delta_dist_x = (ray_dir_x == 0.0f) ? 1e30f : std::abs(1.0f / ray_dir_x);
        const auto delta_dist_y = (ray_dir_y == 0.0f) ? 1e30f : std::abs(1.0f / ray_dir_y);

        // step_* = which neighbor we enter; side_dist_* = ray distance to the *next* vertical/horizontal line from the camera.
        auto step_x = 0;
        auto step_y = 0;
        auto side_dist_x = 0.0f;
        auto side_dist_y = 0.0f;

        // First crossing: how far *along the ray* to the next vertical line vs next horizontal line.
        if (ray_dir_x < 0.0f) {
            step_x = -1;
            side_dist_x = (camera_x - static_cast<float>(map_x)) * delta_dist_x; // distance along ray to left cell edge
        } else {
            step_x = 1;
            side_dist_x = (static_cast<float>(map_x + 1) - camera_x) * delta_dist_x; // …to right edge (dir_x==0: delta blew up, doesn’t matter)
        }
        if (ray_dir_y < 0.0f) { // y decreasing = “up” on the map in this project
            step_y = -1;
            side_dist_y = (camera_y - static_cast<float>(map_y)) * delta_dist_y; // to top edge of current cell
        } else {
            step_y = 1;
            side_dist_y = (static_cast<float>(map_y + 1) - camera_y) * delta_dist_y; // …to bottom edge
        }

        auto hit = false;
        // Which kind of wall we brushed first — 0 = vertical grid line (NS strip), 1 = horizontal (EW strip); used for shade.
        auto side = 0;
        constexpr auto kMaxSteps = 128; // sanity cap — stop if we’d march into infinity on huge maps
        // Race the two “next crossing” distances; smaller one = which wall edge we hit first.
        for (auto steps = 0; steps < kMaxSteps; ++steps) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x; // jump to next vertical grid line
                map_x += step_x; // we’re now standing in the neighbor cell on X
                side = 0;
            } else {
                // tied? this branch runs — fine; corner cases are rare
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
            continue; // ray didn’t hit a wall in range — no wall strip this column
        }

        // Fish-eye fix: need depth *along view plane*, not “how long is the laser”. Messy fraction = hit that grid line.
        auto perp_wall_dist = 0.0f;
        if (side == 0) {
            perp_wall_dist =
                (static_cast<float>(map_x) - camera_x + (1.0f - static_cast<float>(step_x)) * 0.5f) / ray_dir_x;
        } else {
            perp_wall_dist =
                (static_cast<float>(map_y) - camera_y + (1.0f - static_cast<float>(step_y)) * 0.5f) / ray_dir_y;
        }
        if (perp_wall_dist <= 0.0f) {
            continue; // behind camera / nonsense — skip column
        }

        // closer wall → taller strip on screen (1/dist-ish vibe).
        const auto line_height = static_cast<int>(static_cast<float>(screen_height) / perp_wall_dist);
        const auto draw_start = std::max(0, (screen_height - line_height) / 2); // clip if strip taller than screen
        const auto draw_end = std::min(screen_height - 1, (screen_height + line_height) / 2);

        // quick fake lighting: NS vs EW reads different without real normals
        auto wall_color = RGB {.r = 190, .g = 190, .b = 190};
        if (side == 1) {
            wall_color = RGB {.r = 140, .g = 140, .b = 140};
        }
        pixels.VLine(x, draw_start, draw_end, wall_color); // one vertical slice for this column — classic raycaster look
    }
}
} // namespace

auto main() -> int {
    auto level = Level {};
    auto player = Player { level }; // remembers level for bumping into walls + drawing dot on minimap

    // Bitmap we scribble into each frame; GL just textures a quad with this later.
    auto pixels = Pixels { level.Width(), level.Height(), "Raycaster OpenGL" };

    // WASD + arrows; dt scales turns/moves so 30fps vs 144fps feels the same.
    pixels.Update([&](const double delta) {
        const auto move_forward = pixels.IsKeyDown(GLFW_KEY_W) || pixels.IsKeyDown(GLFW_KEY_UP);
        const auto move_backward = pixels.IsKeyDown(GLFW_KEY_S) || pixels.IsKeyDown(GLFW_KEY_DOWN);
        const auto turn_left = pixels.IsKeyDown(GLFW_KEY_A) || pixels.IsKeyDown(GLFW_KEY_LEFT);
        const auto turn_right = pixels.IsKeyDown(GLFW_KEY_D) || pixels.IsKeyDown(GLFW_KEY_RIGHT);

        player.Update(delta, move_forward, move_backward, turn_left, turn_right);
    });

    // Order matters: sky/floor → walls → HUD on top.
    pixels.Draw([&]() {
        DrawBackground(pixels);
        DrawRaycastScene(level, player, pixels);

        // Small corner map; tile_size zooms it (bigger number = chunkier blocks).
        constexpr auto minimap_tile_size = 8u;
        constexpr auto minimap_offset_x = 16u;
        constexpr auto minimap_offset_y = 16u;
        level.DrawMinimap(pixels, minimap_tile_size, minimap_offset_x, minimap_offset_y);
        player.DrawMinimap(pixels, minimap_tile_size, minimap_offset_x, minimap_offset_y);
    });

    pixels.Run(); // blocks until window closes — game loop + GL swap lives inside Window/Pixels

    return 0;
}
