#pragma once

#include <functional>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/timer.hpp"

using ResizeCallback = std::function<void(const int, const int)>;

// GLFW window + OpenGL context; drives the main loop and exposes key state.
class Window {
public:
    Window(unsigned width, unsigned height, std::string_view title);

    // Blocks until the window closes; invokes program(delta_seconds) each frame after polling input.
    auto Start(const std::function<void(const double delta)>& program) -> void;
    auto IsKeyDown(int key) const -> bool;

    ~Window();

private:
    GLFWwindow* window_ {nullptr};
    Timer timer_;
};
