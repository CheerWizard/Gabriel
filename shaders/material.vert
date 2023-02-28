#version 420 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;

out vec3 f_pos;
out vec2 f_uv;
out vec4 direct_light_space_pos;

out vec3 tangent_pos;
out vec3 tangent_normal;
out vec3 tangent_view_pos;
out mat3 TBN;

layout (std140, binding = 0) uniform Camera {
    mat4 perspective;
    mat4 view;
};

uniform mat4 model;
uniform vec3 view_position;
uniform mat4 direct_light_space;

void main()
{
    vec4 world_pos = model * vec4(v_pos, 1.0);
    vec4 world_normal = transpose(inverse(model)) * vec4(v_normal, 0);

    f_pos = world_pos.xyz;
    f_uv = v_uv;
    direct_light_space_pos = direct_light_space * world_pos;

    vec3 T = normalize(mat3(model) * v_tangent);
    vec3 N = normalize(mat3(model) * v_normal);
    T = normalize(T - dot(T, N) * N); // reorthogonolize T to N
    vec3 B = cross(N, T);
    TBN = transpose(mat3(T, B, N));

    tangent_pos = TBN * world_pos.xyz;
    tangent_normal = TBN * world_normal.xyz;
    tangent_view_pos = TBN * view_position;

    gl_Position = perspective * view * world_pos;
}