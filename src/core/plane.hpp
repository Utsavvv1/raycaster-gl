#pragma once

#include <memory>
#include <vector>

// =============================================================================
// Plane — parametric grid in XY (used here as a textured quad for Pixels)
// =============================================================================
//
// Builds a width×height rectangle centered at origin in the z=0 plane, subdivided into
// width_segments × height_segments cells. Each cell → two triangles (CCW from +Z).
//
// Outputs match Mesh’s interleaved layout (8 floats / vertex). UVs map [0,1] across the quad;
// v is flipped so CPU top-left bitmap maps intuitively when sampled in GL.
//
class Plane {
public:
    const float width;
    const float height;
    const unsigned width_segments;
    const unsigned height_segments;

    Plane(
        float width,
        float height,
        unsigned width_segments,
        unsigned height_segments
    );

    [[nodiscard]] auto vertices() const { return vertices_; }
    [[nodiscard]] auto indices() const { return indices_; }

private:
    std::vector<float> vertices_;
    std::vector<unsigned> indices_;
};
