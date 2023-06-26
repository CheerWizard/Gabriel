#version 460 core

#include core.glsl
#include camera.glsl

in vec2 l_uv;
in vec3 w_pos;
in vec3 w_normal;
in vec4 dls_pos;
in vec3 v_pos;
in vec3 v_normal;

layout(location = 0) out vec4 out_position; // position
layout(location = 1) out vec4 out_normal; // normal
layout(location = 2) out vec4 out_albedo; // albedo + transparency
layout(location = 3) out vec4 out_pbr_params; // metalness + roughness + occlusion
layout(location = 4) out vec4 out_emission;
layout(location = 5) out vec4 out_shadow_proj_coords; // shadow proj coords
layout(location = 6) out vec4 out_view_position;
layout(location = 7) out vec4 out_view_normal;

#include features/material.glsl
#include features/parallax.glsl

void main()
{
    UV = l_uv;
    V = normalize(camera_pos - w_pos);
    vec3 world_normal = normalize(w_normal);
    vec3 view_normal = normalize(v_normal);

    // parallax mapping
    if (material.enable_parallax) {
        UV = parallax_function(l_uv);
        if (UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0)
            discard;
    }

    // normal mapping
    if (material.enable_normal) {
        vec3 tangentNormal = texture(material.normal, UV).xyz * 2.0 - 1.0;
        {
            vec3 Q1  = dFdx(w_pos);
            vec3 Q2  = dFdy(w_pos);
            vec2 st1 = dFdx(UV);
            vec2 st2 = dFdy(UV);

            vec3 T = normalize(Q1*st2.t - Q2*st1.t);
            vec3 B = -normalize(cross(world_normal, T));
            mat3 TBN = mat3(T, B, world_normal);

            world_normal = normalize(TBN * tangentNormal);
        }
        {
            vec3 Q1  = dFdx(v_pos);
            vec3 Q2  = dFdy(v_pos);
            vec2 st1 = dFdx(UV);
            vec2 st2 = dFdy(UV);

            vec3 T = normalize(Q1*st2.t - Q2*st1.t);
            vec3 B = -normalize(cross(view_normal, T));
            mat3 TBN = mat3(T, B, view_normal);

            view_normal = normalize(TBN * tangentNormal);
        }
    }

    // albedo mapping
    vec4 albedo = material.color;
    if (material.enable_albedo) {
        albedo *= texture(material.albedo, UV);
    }
    // gamma correction
    albedo = vec4(pow(albedo.rgb, vec3(2.2)), albedo.a);

    // metal mapping
    float metallic = material.metallic_factor;
    if (material.enable_metallic) {
        metallic *= texture(material.metallic, UV).r;
    }

    // roughness mapping
    float roughness = material.roughness_factor;
    if (material.enable_roughness) {
        roughness *= texture(material.roughness, UV).r;
    }

    // AO mapping
    float ao = material.ao_factor;
    if (material.enable_ao) {
        ao *= texture(material.ao, UV).r;
    }

    // emission mapping
    vec3 emission = material.emission_factor;
    if (material.enable_emission) {
        emission *= texture(material.emission, UV).rgb;
    }
    // tone mapping
    emission = vec3(pow(emission.rgb, vec3(2.2)));

    out_position = vec4(w_pos, 1.0);
    out_normal = vec4(world_normal, 1.0);
    out_albedo = albedo;
    out_pbr_params = vec4(metallic, roughness, ao, 1.0);
    out_emission = vec4(emission, 1.0);
    out_shadow_proj_coords = vec4((dls_pos.xyz / dls_pos.w) * 0.5 + 0.5, 1.0);
    out_view_position = vec4(v_pos, 1.0);
    out_view_normal = vec4(view_normal, 1.0);
}