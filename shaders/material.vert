#version 460 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_tangent;

out vec2 f_uv;
out vec4 direct_light_space_pos;

out vec3 world_pos;
out vec3 world_normal;
out vec3 world_view_pos;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 model;
uniform vec3 view_position;
uniform mat4 direct_light_space;

void main()
{
    world_pos = (model * vec4(v_pos, 1.0)).xyz;
    world_normal = (transpose(inverse(model)) * vec4(v_normal, 0)).xyz;
    world_view_pos = view_position;

    f_uv = v_uv;
    direct_light_space_pos = direct_light_space * vec4(world_pos, 1.0);

    gl_Position = perspective * view * vec4(world_pos, 1.0);
}