#pragma once

#include <stdexcept>
#include <string_view>
#include <vector>

#include <glad/glad.h>

enum class ShaderType {
    kVertexShader,
    kFragmentShader
};

struct ShaderInfo {
    ShaderType type;
    std::string_view source;
};

// =============================================================================
// Shader — compile/link OpenGL program from GLSL source strings
// =============================================================================
//
// Constructor pipeline:
//   glCreateProgram → for each stage: compile, attach → glLinkProgram.
// Compile/link failures throw ShaderError with driver log text on stderr.
//
// Uniform helpers call Use() internally because uniform locations are program-relative.
//
class Shader {
public:
    explicit Shader(const std::vector<ShaderInfo>& shaders);

    auto Use() const -> void;

    auto GetUniform(std::string_view name) const -> GLint;
    auto SetUniform(std::string_view uniform, int i) const -> void;
    auto SetUniform(std::string_view uniform, const float f) const -> void;

    ~Shader();

private:
    GLuint program_;

    auto GetShaderType(ShaderType type) const -> GLuint;
    auto GetShaderTypeString(ShaderType type) const -> const char*;
    auto CheckShaderCompileStatus(GLuint shader_id, ShaderType type) const -> void;
    auto CheckProgramLinkStatus() const -> void;
};

struct ShaderError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
