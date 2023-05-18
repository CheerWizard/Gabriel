#version 460 core

#include core.glsl
#include camera.glsl

in vec2 l_uv;
in vec3 w_pos;
in vec3 w_normal;
in vec4 dls_pos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float outReveal;

#include features/material.glsl
#include features/parallax.glsl

#include features/lighting/direct.glsl
#include features/lighting/point.glsl
#include features/lighting/spot.glsl

#include features/shadow/direct.glsl
#include features/shadow/point.glsl

#include pbr/core.glsl
#include pbr/direct.glsl
#include features/lighting/env.glsl

void main()
{
    UV = l_uv;
    N = normalize(w_normal);
    V = normalize(camera_pos - w_pos);
    float NdotV = max(dot(N, V), 0.0);

    // parallax mapping
    if (material.enable_parallax) {
        UV = parallax_function(l_uv);
        if (UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0)
            discard;
    }

    // normal mapping
    if (material.enable_normal) {
        vec3 tangentNormal = texture(material.normal, UV).xyz * 2.0 - 1.0;

        vec3 Q1  = dFdx(w_pos);
        vec3 Q2  = dFdy(w_pos);
        vec2 st1 = dFdx(UV);
        vec2 st2 = dFdy(UV);

        vec3 T = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        N = normalize(TBN * tangentNormal); // tangent space -> view space
    }

    R = reflect(-V, N);

    // albedo mapping
    vec4 albedo = material.color;
    if (material.enable_albedo) {
        albedo *= texture(material.albedo, UV);
    }
    // tone mapping
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
    emission = vec3(pow(emission, vec3(2.2)));

    // PBR
    vec3 Lo = vec3(0);

    // apply direct lights
    int directLightSize = directLights.length();
    for (int i = 0 ; i < directLightSize ; i++) {
        Lo += pbr(directLights[i], albedo.rgb, metallic, roughness);
    }

    // apply point lights
    int pointLightSize = pointLights.length();
    for (int i = 0 ; i < pointLightSize ; i++) {
        Lo += pbr(pointLights[i], albedo.rgb, metallic, roughness);
    }

    // apply spot lights
    int spotLightSize = spotLights.length();
    for (int i = 0 ; i < spotLightSize ; i++) {
        Lo += pbr(spotLights[i], albedo.rgb, metallic, roughness);
    }

    // apply emissive light
    Lo += pbr(N, emission, 1.0, albedo.rgb, metallic, roughness);

    vec3 ambient = ibl(NdotV, N, R, albedo.rgb, metallic, roughness);

    outColor = vec4((ambient + Lo) * ao, albedo.a);
}