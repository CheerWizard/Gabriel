#version 420 core

layout (location = 0) in vec3 v_pos;

out vec3 f_coords;

uniform mat4 perspective;
uniform mat4 view;

void main()
{
    f_coords = v_pos;
    vec4 clip_pos = perspective * view * vec4(v_pos, 1.0);
    gl_Position = clip_pos.xyww;
}