#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 2) in vec3 a_normal;

out NormalVertex {
    vec3 a_normal;
} vertex_out;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(a_pos, 1.0);
    vertex_out.a_normal = a_normal;
}