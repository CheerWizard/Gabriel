#version 460 core

out float out_color;

in vec2 l_uv;

#include camera.glsl

uniform sampler2D v_positions;
uniform sampler2D v_normals;
uniform sampler2D noise;

uniform vec2 resolution;
uniform int noise_size;
uniform int samples_size;
uniform float sample_radius;
uniform float sample_bias;  // used to solve "acne" visual bug
uniform int occlusion_power;

uniform vec3 samples[64];

void main() {
    vec3 pos       = texture(v_positions, l_uv).xyz;
    vec3 N         = texture(v_normals, l_uv).rgb;
    vec3 random    = texture(noise, l_uv * resolution / noise_size).xyz;

    vec3 tangent   = normalize(random - N * dot(random, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN       = mat3(tangent, bitangent, N);

    float occlusion = 0.0;
    for (int i = 0; i < samples_size; ++i)
    {
        // get sample position
        vec3 sample_pos = TBN * samples[i];   // from tangent space to view space
        sample_pos = pos + sample_pos * sample_radius;
        vec4 offset = vec4(sample_pos, 1.0);
        offset      = perspective * offset;   // from view space to clip space
        offset.xyz /= offset.w;               // from clip space to normalized device space
        offset.xyz  = offset.xyz * 0.5 + 0.5; // [-1, 1] -> [0, 1]
        float sample_depth = texture(v_positions, offset.xy).z;
        float range = smoothstep(0.0, 1.0, sample_radius / abs(pos.z - sample_depth));
        occlusion += range * (sample_depth >= sample_pos.z + sample_bias ? 1.0 : 0.0);
    }
    occlusion = 1.0 - (occlusion / samples_size);
    out_color = pow(occlusion, occlusion_power);
}