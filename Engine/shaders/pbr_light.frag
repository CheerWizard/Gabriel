#version 460 core

#include core.glsl
#include camera.glsl

in vec2 l_uv;

vec3 w_pos;

layout(location = 0) out vec4 outColor;

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D albedos;
uniform sampler2D pbr_params;
uniform sampler2D emissions;

uniform sampler2D ssao;
uniform bool enable_ssao;

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
    vec4 w_positions = texture(positions, l_uv);
    vec4 w_normals = texture(normals, l_uv);
    vec4 albedo = texture(albedos, l_uv);
    vec4 pbr_param = texture(pbr_params, l_uv);
    vec3 emission = texture(emissions, l_uv).rgb;

    w_pos = w_positions.xyz;
    N = w_normals.xyz;
    V = normalize(camera_pos - w_pos);
    R = reflect(-V, N);
    float NdotV = max(dot(N, V), 0.0);

    float metallic = pbr_param.r;
    float roughness = pbr_param.g;
    float ao = pbr_param.b;
    vec3 Lo = vec3(0);

    // SSAO
    if (enable_ssao) {
        ao *= texture(ssao, l_uv).r;
    }

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