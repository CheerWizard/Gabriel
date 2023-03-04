#version 460 core

layout (location = 0) in vec3 v_pos;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(v_pos, 1.0);
}