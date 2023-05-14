#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 l_uv;

#include camera.glsl

uniform mat4 model;

void main()
{
    l_uv = a_uv;
    gl_Position = perspective * view * model * vec4(a_pos, 1.0);
}