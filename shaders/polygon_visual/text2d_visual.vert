#version 460 core

#include ../core.glsl
#include ../camera.glsl

layout(location = 0) in vec2 a_pos;

uniform mat3 model;

uniform float thickness = 1.0005f;

void main() {
    gl_Position = vec4(model * vec3(a_pos * thickness, 0), 1);
}
