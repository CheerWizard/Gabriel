#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;

out vec3 f_pos;
out vec2 f_uv;
out vec3 f_normal;
out vec3 f_light_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform mat4 normal_model;

uniform vec3 light_position;

void main()
{
    vec4 world_pos = model * vec4(v_pos, 1.0);
    vec4 view_pos = view * world_pos;
    vec4 view_normal = transpose(inverse(view * model)) * vec4(v_normal, 0);
    vec4 view_light_pos = view * vec4(light_position, 1.0);

    f_pos = view_pos.xyz;
    f_normal = view_normal.xyz;
    f_uv = v_uv;

    gl_Position = perspective * view_pos;
}