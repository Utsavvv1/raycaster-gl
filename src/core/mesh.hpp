// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#pragma once

#include <vector>

#include "core/shader.hpp"

class Mesh {
public:
    Mesh(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data = {}
    );

    auto Draw(const Shader& shader) const -> void;

private:
    unsigned vao_ {0};
    unsigned vbo_ {0};
    unsigned ebo_ {0};
    unsigned vertex_size_ {0};
    unsigned index_size_ {0};

    auto Init(
        const std::vector<float>& vertex_data,
        const std::vector<unsigned int>& index_data = {}
    ) -> void;

    auto ConfigureVertices(const std::vector<float>& vertex_data) -> void;

    auto ConfigureIndices(const std::vector<unsigned int>& index_data) -> void;
};