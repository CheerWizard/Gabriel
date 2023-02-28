#version 420 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;

out vec4 f_color;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 model;

void main()
{
    gl_Position = perspective * view * model * vec4(v_pos, 1.0);
    f_color = v_color;
}