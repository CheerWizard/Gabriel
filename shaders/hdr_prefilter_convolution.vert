#version 420 core

layout (location = 0) in vec3 v_pos;

out vec3 f_pos;

uniform mat4 perspective;
uniform mat4 view;

void main()
{
    f_pos = v_pos;
    gl_Position = perspective * view * vec4(v_pos, 1.0);
}
