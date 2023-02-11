#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;

out vec4 f_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{
    gl_Position = perspective * view * model * vec4(v_pos, 1.0);
    f_color = v_color;
}