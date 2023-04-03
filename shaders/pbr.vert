#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;

out vec2 l_uv;
out vec3 w_pos;
out vec3 w_normal;
out vec4 dls_pos;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 model;
uniform mat4 direct_light_space;

void main()
{
    l_uv = a_uv;
    w_pos = (model * vec4(a_pos, 1.0)).xyz;
    w_normal = transpose(inverse(mat3(model))) * a_normal;
    dls_pos = direct_light_space * vec4(w_pos, 1.0);

    gl_Position = perspective * view * vec4(w_pos, 1.0);
}