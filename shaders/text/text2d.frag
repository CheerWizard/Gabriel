#version 460 core

layout(location = 0) out vec4 out_color;

in vec2 l_uv;

uniform sampler2D bitmap;

uniform vec4 text_color;

void main() {
    vec4 character = vec4(1, 1, 1, 1);
    out_color = character;
}