#version 460 core

#include ../core.glsl
#include ../camera.glsl

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 l_uv;

uniform mat3 model;

void main() {
    vec3 w_pos = model * vec3(a_pos, 0);
    gl_Position = vec4(w_pos, 1);
    l_uv = a_uv;
}