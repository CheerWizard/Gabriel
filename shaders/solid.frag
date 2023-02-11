#version 330 core

out vec4 f_base_color;

in vec4 f_color;

void main()
{
    f_base_color = f_color;
}