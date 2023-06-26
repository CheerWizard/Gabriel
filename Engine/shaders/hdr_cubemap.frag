#version 460 core

in vec3 l_pos;

layout(location = 0) out vec4 out_color;

uniform samplerCube sampler;

void main() {
    out_color = vec4(textureLod(sampler, l_pos, 0.0).rgb, 1.0);
}
