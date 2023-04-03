#version 460 core

in vec2 l_uv;

out vec4 out_color;

uniform sampler2D sampler;

void main() {
    out_color = texture(sampler, l_uv);
}