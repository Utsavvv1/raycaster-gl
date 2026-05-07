#pragma once

#include <vector>

#include "core/shader.hpp"

// =============================================================================
// Mesh — VAO + optional indexed draw for static geometry
// =============================================================================
//
// Vertex layout (interleaved float array, VERTEX_ELEMENTS = 8 per vertex):
//   [0..2] position xyz
//   [3..5] normal   xyz   (unused by current shaders but matches attrib layout)
//   [6..7] uv       st
//
// Init binds a VAO, uploads VBO (+ optional EBO), records attrib pointers on the VAO, then
// deletes the VBO/EBO *names* — the drivers keep storage alive because the VAO references them.
//
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
