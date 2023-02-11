#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;

out vec4 f_color;

void main()
{
    gl_Position = vec4(v_pos.x + 0.5, v_pos.y, v_pos.z, 1.0);
    f_color = v_color;
}