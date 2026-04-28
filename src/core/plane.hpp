// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#pragma once

#include <memory>
#include <vector>

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