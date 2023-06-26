#version 460 core

#include ../core.glsl
#include ../camera.glsl

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 l_uv;

uniform mat3 model;

void main() {
    gl_Position = vec4(model * vec3(a_pos, 0), 1);
    l_uv = a_uv;
}