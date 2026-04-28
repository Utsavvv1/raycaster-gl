// Copyright 2024 Betamark Pty Ltd. All rights reserved.
// Author: Shlomi Nissan (shlomi@betamark.com)

#include "pixels.hpp"

#include <algorithm>
#include <cmath>

#include "core/plane.hpp"

#include "shaders/headers/vertex.h"
#include "shaders/headers/fragment.h"

constexpr unsigned RGB_SIZE = 3;

Pixels::Pixels(unsigned width, unsigned height, std::string_view title)
  : data_(width * height * RGB_SIZE, 0),
    width_(width), height_(height),
    window_(width, height, title),
    shader_ {{
        {ShaderType::kVertexShader, _SHADER_vertex},
        {ShaderType::kFragmentShader, _SHADER_fragment}
    }} {

    auto plane = Plane {2, 2, 1, 1};
    screen_ = std::make_unique<Mesh>(plane.vertices(), plane.indices());

    InitTexture();
}

auto Pixels::Run() -> void {
    window_.Start([&](const double delta) {
        Clear();

        if (update_) update_(delta);
        if (draw_) draw_();

        Bind();
        screen_->Draw(shader_);
    });
}

auto Pixels::InitTexture() -> void {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width_, height_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, data_.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

auto Pixels::SetStroke(RGB color) -> void {
    stroke_color_ = color;
    no_stroke_ = false;
}

auto Pixels::SetFill(RGB color) -> void {
    fill_color_ = color;
    no_fill_ = false;
}

auto Pixels::PutPixel(unsigned x, unsigned y, RGB color) -> void {
    if (x >= width_ || y >= height_) return;

    y = height_ - y - 1; // flip the y coordinate

    auto index = (y * width_ + x) * RGB_SIZE;
    data_[index] = color.r;
    data_[index + 1] = color.g;
    data_[index + 2] = color.b;
}

auto Pixels::Line(unsigned x1, unsigned y1, unsigned x2, unsigned y2) -> void {
    if (no_stroke_) return;

    auto delta_x = static_cast<int>(x2 - x1);
    auto delta_y = static_cast<int>(y2 - y1);
    auto sign_x = delta_x < 0 ? -1 : 1;
    auto sign_y = delta_y < 0 ? -1 : 1;
    auto steep = std::abs(delta_y) >= std::abs(delta_x);
    auto slope = steep ?
        static_cast<float>(delta_x) / static_cast<float>(delta_y) :
        static_cast<float>(delta_y) / static_cast<float>(delta_x);

    if (steep) {
        for (auto i = 0; i < delta_y; ++i) {
            auto px = slope * i + x1;
            auto py = i + y1;
            PutPixel(px, py, stroke_color_);
        }
    } else {
        for (auto i = 0; i < delta_x; ++i) {
            auto px = i + x1;
            auto py = slope * i + y1;
            PutPixel(px, py, stroke_color_);
        }
    }
}

auto Pixels::Rect(unsigned x, unsigned y, unsigned width, unsigned height) -> void {
    if (no_stroke_ && no_fill_) {
        return;
    }

    if (x >= width_ || y >= height_) {
        return;
    }

    if (x + width >= width_) width = width_ - x;
    if (y + height >= height_) height = height_ - y;

    const auto color = no_stroke_ ? fill_color_ : stroke_color_;
    for (unsigned i = 0; i < width; ++i) {
        PutPixel(x + i, y, color);
        PutPixel(x + i, y + height - 1, color);
    }
    for (unsigned j = 0; j < height; ++j) {
        PutPixel(x, y + j, color);
        PutPixel(x + width - 1, y + j, color);
    }

    if (no_fill_) return;

    y = height_ - y - height; // flip the y coordinate

    for (unsigned py = y + 1; py < y + height - 1; ++py) {
        auto index = (py * width_ + x + 1) * RGB_SIZE;
        auto end = index + (width - 2) * RGB_SIZE;
        for (unsigned px = index; px < end; px += RGB_SIZE) {
            data_[px] = fill_color_.r;
            data_[px + 1] = fill_color_.g;
            data_[px + 2] = fill_color_.b;
        }
    }
}

auto Pixels::Clear() -> void {
    std::fill(begin(data_), end(data_), 0);
}

auto Pixels::Bind() -> void {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, data_.data());
}

Pixels::~Pixels() {
    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
    }
}