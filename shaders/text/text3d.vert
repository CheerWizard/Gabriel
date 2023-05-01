#version 460 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 l_uv;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 model;

void main() {
    gl_Position = perspective * view * model * vec4(a_pos, 0, 1);
    l_uv = a_uv;
}