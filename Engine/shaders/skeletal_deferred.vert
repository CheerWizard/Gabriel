#version 460 core

#include core.glsl
#include camera.glsl

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec3 a_tangent;
layout (location = 5) in ivec4 a_bone_id;
layout (location = 6) in vec4 a_weight;

out vec2 l_uv;
out vec3 w_pos;
out vec3 w_normal;
out vec4 dls_pos;
out vec3 v_pos;
out vec3 v_normal;

uniform mat4 model;
uniform mat4 direct_light_space;

const int MAX_BONES = 100;
uniform mat4 bone_transforms[MAX_BONES];

void main()
{
    l_uv = a_uv;

    vec4 l_pos = vec4(0);
    mat4 bone_transform = mat4(0);

    for (int i = 0 ; i < 4 ; i++) {
        int b_id = a_bone_id[i];

        if (b_id == -1) continue;
        if (b_id >= MAX_BONES) {
            l_pos = vec4(a_pos, 1.0);
            break;
        }

        mat4 bone = bone_transforms[b_id] * a_weight[b_id];
        l_pos += bone * vec4(a_pos, 1.0);
        bone_transform += bone;
    }

    w_pos = (model * l_pos).xyz;
    v_pos = (view * vec4(w_pos, 1.0)).xyz;

    w_normal = transpose(inverse(mat3(bone_transform))) * a_normal;
    v_normal = transpose(inverse(mat3(view * bone_transform))) * a_normal;

    dls_pos = direct_light_space * vec4(w_pos, 1.0);

    gl_Position = perspective * vec4(v_pos, 1.0);
}