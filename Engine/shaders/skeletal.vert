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

uniform mat4 model;
uniform mat4 direct_light_space;

const int MAX_BONES = 100;
uniform mat4 bone_transforms[MAX_BONES];

void main()
{
    l_uv = a_uv;

    vec4 totalPosition = vec4(0.0f);

    for (int i = 0 ; i < 4 ; i++) {
        if (a_bone_id[i] == -1)
            continue;

        if (a_bone_id[i] >= MAX_BONES) {
            totalPosition = vec4(a_pos, 1.0f);
            break;
        }

        vec4 localPosition = bone_transforms[a_bone_id[i]] * vec4(a_pos, 1.0f);
        totalPosition += localPosition;
        vec3 localNormal = mat3(bone_transforms[a_bone_id[i]]) * a_normal;
    }

    w_pos = (model * totalPosition).xyz;

    w_normal = transpose(inverse(mat3(model))) * a_normal;

    dls_pos = direct_light_space * vec4(w_pos, 1.0);

    gl_Position = perspective * view * vec4(w_pos, 1.0);
}