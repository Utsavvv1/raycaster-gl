// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include <glad/glad.h>

#include "core/mesh.hpp"
#include "core/window.hpp"
#include "core/shader.hpp"

struct RGB {
    unsigned char r {0};
    unsigned char g {0};
    unsigned char b {0};
};

using DrawCallback = std::function<void()>;
using UpdateCallback = std::function<void(const double delta)>;

class Pixels {
public:
    Pixels(unsigned width, unsigned height, std::string_view title);

    ~Pixels();

    auto Draw(const DrawCallback& c) { draw_ = c; }

    auto Update(const UpdateCallback& c) { update_ = c; }

    auto Run() -> void;

    auto ResetStroke() -> void;

    auto SetStroke(RGB color) -> void;

    auto NoStroke() { no_stroke_ = true; }

    auto ResetFill() -> void;

    auto SetFill(RGB color) -> void;

    auto NoFill() { no_fill_ = true; }

    auto Line(unsigned x1, unsigned y1, unsigned x2, unsigned y2) -> void;

    auto Rect(unsigned x, unsigned y, unsigned width, unsigned height) -> void;

    auto Clear() -> void;

    auto Bind() -> void;

private:
    bool no_stroke_ {false};
    bool no_fill_ {false};

    unsigned texture_ {0};
    unsigned width_ {0};
    unsigned height_ {0};

    RGB stroke_color_ {255, 255, 255};
    RGB fill_color_ {255, 255, 255};

    UpdateCallback update_;
    DrawCallback draw_;

    std::vector<unsigned char> data_;

    std::unique_ptr<Mesh> screen_;

    Window window_;
    Shader shader_;

    auto InitTexture() -> void;

    auto PutPixel(unsigned x, unsigned y, RGB color) -> void;
};