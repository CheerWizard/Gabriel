#version 460 core

#include ../core.glsl

layout(location = 0) out vec4 out_color;

in vec2 l_uv;

uniform sampler2D bitmap;

uniform vec4 textColor;

void main() {
    out_color = vec4(textColor.rgb, texture(bitmap, l_uv).a * textColor.a);
}