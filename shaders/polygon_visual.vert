#version 460 core

#include core.glsl
#include camera.glsl

layout (location = 0) in vec3 a_pos;

uniform mat4 model;

uniform float thickness = 1.0005f;

void main()
{
    gl_Position = perspective * view * model * vec4(a_pos * thickness, 1.0);
}