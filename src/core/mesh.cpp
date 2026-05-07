// mesh.cpp — Mesh VAO/VBO setup (see mesh.hpp).

#include "mesh.hpp"

#include <glad/glad.h>

#define BUFFER_OFFSET(offset) ((void*)(offset * sizeof(GLfloat)))
#define STRIDE(stride) (sizeof(GLfloat) * stride)
#define VERTEX_ELEMENTS 8

Mesh::Mesh(
    const std::vector<float>& vertex_data,
    const std::vector<unsigned int>& index_data
) {
    Init(vertex_data, index_data);
}

auto Mesh::Init(
    const std::vector<float>& vertex_data,
    const std::vector<unsigned int>& index_data
) -> void {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    ConfigureVertices(vertex_data);
    if (!index_data.empty()) {
        ConfigureIndices(index_data);
    }

    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

auto Mesh::Draw(const Shader& shader) const -> void {
    shader.Use();

    glBindVertexArray(vao_);
    if (index_size_ > 0) {
        glDrawElements(GL_TRIANGLES, index_size_, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, vertex_size_);
    }
}

auto Mesh::ConfigureVertices(const std::vector<float>& vertex_data) -> void {
    vertex_size_ = vertex_data.size() / VERTEX_ELEMENTS;

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertex_data.size() * sizeof(float),
        vertex_data.data(),
        GL_STATIC_DRAW
    );

    // Location 0 — vec3 position.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE(8), BUFFER_OFFSET(0));

    // Location 1 — vec3 normal (fragment shader ignores for now).
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE(8), BUFFER_OFFSET(3));

    // Location 2 — vec2 UV → sampled in fragment shader as Texture0.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE(8), BUFFER_OFFSET(6));
}

auto Mesh::ConfigureIndices(const std::vector<unsigned int>& index_data) -> void {
    index_size_ = index_data.size();

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        index_data.size() * sizeof(unsigned int),
        index_data.data(),
        GL_STATIC_DRAW
    );
}
