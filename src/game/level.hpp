#pragma once

#include <vector>

#include "core/pixels.hpp"

// =============================================================================
// Level — static tile map for walls + collision / ray queries
// =============================================================================
//
// Representation:
//   • data_[row][column] where row grows downward (same as screen/minimap y).
//   • Value 1 = solid wall, 0 = empty air the player can occupy.
//
// Space conversions:
//   • Tile indices (tile_x, tile_y) identify one cell in this grid.
//   • World pixels = tile index * TILE_SIZE + offset inside the tile (Player uses this).
//
// The raycaster in main.cpp converts world → tile space by dividing by TILE_SIZE.
//
class Level {
public:
    static constexpr unsigned TILE_SIZE = 64;

    auto Rows() const { return static_cast<unsigned>(data_.size()); }

    auto Cols() const { return static_cast<unsigned>(data_.front().size()); }

    // Total extent of the level bitmap in pixels (not the window — window matches this).
    auto Width() const { return Cols() * Level::TILE_SIZE; }

    auto Height() const { return Rows() * Level::TILE_SIZE; }

    // Minimap: each map tile drawn as a small square; optional pixel offset for HUD placement.
    auto DrawMinimap(Pixels& pixels) const -> void;
    auto DrawMinimap(Pixels& pixels, unsigned tile_size, unsigned offset_x, unsigned offset_y) const -> void;

    auto IsInBounds(int tile_x, int tile_y) const -> bool;
    auto IsWallTile(int tile_x, int tile_y) const -> bool;

    // True if the world-pixel point lies inside a wall cell (used for movement collision).
    auto IsWallAtWorld(float world_x, float world_y) const -> bool;

private:
    std::vector<std::vector<int>> data_ {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

};
