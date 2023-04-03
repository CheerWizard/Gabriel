#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 l_uv;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 models[16];

void main()
{
    l_uv = a_uv;
    gl_Position = perspective * view * models[gl_InstanceID] * vec4(a_pos, 1.0);
}