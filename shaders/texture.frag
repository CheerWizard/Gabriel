#version 460 core

in vec2 l_uv;

out vec4 outColor;

uniform sampler2D sampler;

void main() {
    outColor = texture(sampler, l_uv);
}