#version 420 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;

out vec3 f_pos;
out vec2 f_uv;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 models[16];

void main()
{
    vec4 world_pos = models[gl_InstanceID] * vec4(v_pos, 1.0);

    f_pos = world_pos.xyz;
    f_uv = v_uv;

    gl_Position = perspective * view * world_pos;
}