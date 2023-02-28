#version 420 core

in vec2 f_uv;

out vec4 fragment;

uniform sampler2D sampler;

void main() {
    vec4 color = texture(sampler, f_uv);
    fragment = color;
}