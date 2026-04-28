// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#pragma once

#include <functional>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/timer.hpp"

using ResizeCallback = std::function<void(const int, const int)>;

class Window {
public:
    Window(unsigned width, unsigned height, std::string_view title);

    auto Start(const std::function<void(const double delta)>& program) -> void;

    ~Window();

private:
    GLFWwindow* window_ {nullptr};
    Timer timer_;
};