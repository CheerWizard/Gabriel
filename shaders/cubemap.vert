#version 420 core

layout (location = 0) in vec3 v_pos;

out vec3 f_pos;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

void main()
{
    f_pos = v_pos;
    mat4 v = mat4(mat3(view)); // remove translation part
    vec4 clip_pos = perspective * v * vec4(v_pos, 1.0);
    gl_Position = clip_pos.xyww;
}