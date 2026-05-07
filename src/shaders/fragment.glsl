#version 410 core
#pragma debug(on)
#pragma optimize(off)

// Fragment stage: each fullscreen pixel samples one texel of the CPU framebuffer.
//
// Pixels::Bind uploads RGB8 data to GL_TEXTURE_2D on unit 0; Pixels ctor sets uniform "Texture0" to 0.
// GL_NEAREST filtering keeps crisp square pixels (retro look).

layout (location = 0) out vec4 outFragColor;

in vec2 TexCoords;

uniform sampler2D Texture0;

void main() {
    vec3 pixel_color = texture(Texture0, TexCoords).rgb;
    outFragColor = vec4(pixel_color, 1.0);
}
