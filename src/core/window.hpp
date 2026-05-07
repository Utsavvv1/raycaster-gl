#pragma once

// window.hpp — thin GLFW + OpenGL bootstrap for the game.
//
// Owns a GLFW window and the current OpenGL context. Construction sets up
// a fixed-size core-profile context (4.1), loads function pointers via GLAD,
// and enables depth testing. The game runs inside Start(), which owns the
// main loop: clear, user frame callback with delta time, swap buffers, poll events.
// If construction fails (init, context, or GLAD), window_ stays null and Start()
// becomes a no-op so callers can still destroy the object safely.

#include <functional>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/timer.hpp"

// Convenience type for a framebuffer-resize handler (not wired in this class yet).
using ResizeCallback = std::function<void(const int, const int)>;

class Window {
public:
    Window(unsigned width, unsigned height, std::string_view title);

    // Main loop: runs until the user closes the window. Each iteration clears the
    // framebuffer, calls program(delta_seconds) for one frame of game logic and
    // rendering, then presents and processes input. Delta is wall-clock time since
    // the previous frame (variable timestep).
    auto Start(const std::function<void(const double delta)>& program) -> void;

    // GLFW_PRESS for the given GLFW_KEY_* constant; false if the window was never created.
    auto IsKeyDown(int key) const -> bool;

    ~Window();

private:
    GLFWwindow* window_ {nullptr};
    Timer timer_;
};
