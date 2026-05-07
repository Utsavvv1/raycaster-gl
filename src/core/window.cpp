#include "window.hpp"

// window.cpp — GLFW/GLAD setup, frame loop, and teardown for Window.

#include <GLFW/glfw3.h>
#include <fmt/format.h>

// GLFW reports errors asynchronously; this prints them to the console.
constexpr auto callback_error =
[](int error, const char* message) {
    fmt::print("Error ({}): {}\n", error, message);
};

Window::Window(unsigned width, unsigned height, std::string_view title) {
    glfwSetErrorCallback(callback_error);

    if (!glfwInit()) {
        return;
    }

    // Request a forward-compatible core profile so we use the modern pipeline only.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // HiDPI: framebuffer pixels can differ from window coordinates on Retina displays.
    #ifdef __APPLE__
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    #endif

    window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

    if (window_ == nullptr) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window_);
    // Resolve OpenGL entry points for this context (required before calling GL functions).
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        return;
    }

    glfwSwapInterval(1);  // V-sync: SwapBuffers waits for the display refresh.
    glfwSetWindowUserPointer(window_, this);

    // Match the initial viewport to the drawable size (important on scaled monitors).
    auto buffer_width {0};
    auto buffer_height {0};
    glfwGetFramebufferSize(window_, &buffer_width, &buffer_height);
    glViewport(0, 0, buffer_width, buffer_height);

    glEnable(GL_DEPTH_TEST);
}

auto Window::Start(const std::function<void(const double delta)> &program) -> void {
    if (window_ == nullptr) {
        return;
    }

    timer_.Reset();

    while(!glfwWindowShouldClose(window_)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Wall-clock seconds since last iteration; game code uses this for motion (variable dt).
        auto delta = timer_.GetSeconds();
        timer_.Reset();
        program(delta);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

auto Window::IsKeyDown(int key) const -> bool {
    if (window_ == nullptr) {
        return false;
    }

    return glfwGetKey(window_, key) == GLFW_PRESS;
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}
