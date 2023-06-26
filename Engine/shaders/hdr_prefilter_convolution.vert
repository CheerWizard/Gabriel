#version 460 core

layout (location = 0) in vec3 a_pos;

out vec3 l_pos;

uniform mat4 perspective;
uniform mat4 view;

void main()
{
    l_pos = a_pos;
    gl_Position = perspective * view * vec4(a_pos, 1.0);
}
