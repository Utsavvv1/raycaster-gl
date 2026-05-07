#include "level.hpp"

#include <cmath>

auto Level::DrawMinimap(Pixels& pixels) const -> void {
    DrawMinimap(pixels, TILE_SIZE, 0, 0);
}

auto Level::DrawMinimap(Pixels& pixels, unsigned tile_size, unsigned offset_x, unsigned offset_y) const -> void {
    if (tile_size == 0) {
        return;
    }

    pixels.SetStroke({0, 0, 0});
    for (auto i = 0; i < Rows(); ++i) {
        for (auto j = 0; j < Cols(); ++j) {
            const auto x = offset_x + j * tile_size;
            const auto y = offset_y + i * tile_size;
            if (data_[i][j] == 1) {
                pixels.SetFill({0, 0, 0});
            } else {
                pixels.SetFill({255, 255, 255});
            }
            pixels.Rect(x, y, tile_size, tile_size);
        }
    }
}

auto Level::IsInBounds(int tile_x, int tile_y) const -> bool {
    return tile_x >= 0
        && tile_y >= 0
        && tile_x < static_cast<int>(Cols())
        && tile_y < static_cast<int>(Rows());
}

auto Level::IsWallTile(int tile_x, int tile_y) const -> bool {
    // Treat out-of-map as solid so rays and movement cannot escape the grid.
    if (!IsInBounds(tile_x, tile_y)) {
        return true;
    }

    return data_[tile_y][tile_x] == 1;
}

auto Level::IsWallAtWorld(float world_x, float world_y) const -> bool {
    const auto tile_x = static_cast<int>(std::floor(world_x / static_cast<float>(TILE_SIZE)));
    const auto tile_y = static_cast<int>(std::floor(world_y / static_cast<float>(TILE_SIZE)));

    return IsWallTile(tile_x, tile_y);
}
