#version 460 core

layout (location = 0) in vec3 a_pos;

uniform mat4 direct_light_space;
uniform mat4 model;

void main()
{
    gl_Position = direct_light_space * model * vec4(a_pos, 1.0);
}