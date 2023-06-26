#version 460 core

#include core.glsl
#include camera.glsl

layout (location = 0) in vec3 a_pos;

uniform mat4 model;

void main()
{
    gl_Position = perspective * view * model * vec4(a_pos, 1.0);
}