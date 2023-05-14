#version 460 core

#include core.glsl
#include camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 model;

uniform float outline_thickness;

void main()
{
    gl_Position = perspective * view * model * vec4(a_pos + a_normal * outline_thickness, 1.0);
}