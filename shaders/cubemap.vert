#version 460 core

layout (location = 0) in vec3 a_pos;

out vec3 l_pos;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

void main()
{
    l_pos = a_pos;
    mat4 v = mat4(mat3(view)); // remove translation part
    vec4 clip_pos = perspective * v * vec4(a_pos, 1.0);
    gl_Position = clip_pos.xyww;
}