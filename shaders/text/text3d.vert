#version 460 core

#include ../core.glsl
#include ../camera.glsl

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 l_uv;

uniform mat4 model;

void main() {
    gl_Position = perspective * view * model * vec4(a_pos, 0, 1);
    l_uv = a_uv;
}