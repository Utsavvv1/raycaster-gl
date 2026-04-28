#version 410 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) out vec4 outFragColor;

in vec2 TexCoords;

uniform sampler2D Texture0;

void main() {
    vec3 pixel_color = texture(Texture0, TexCoords).rgb;
    outFragColor = vec4(pixel_color, 1.0);
}