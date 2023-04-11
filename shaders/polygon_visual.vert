#version 460 core

layout (location = 0) in vec3 a_pos;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 model;

uniform float thickness = 1.0005f;

void main()
{
    gl_Position = perspective * view * model * vec4(a_pos * thickness, 1.0);
}