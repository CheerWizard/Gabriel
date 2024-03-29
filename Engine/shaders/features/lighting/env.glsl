layout(std140, binding = 5) uniform EnvLight {
    int prefilter_levels;
    samplerCube irradianceSampler;
    samplerCube prefilterSampler;
    sampler2D brdfConvolutionSampler;
};

vec3 ibl(float NdotV, vec3 N, vec3 R, vec3 albedo, float metallic, float roughness) {

    // apply env light
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnel_shlick_rough(NdotV, F0, roughness);

    // indirect diffuse part
    vec3 irradiance = texture(irradianceSampler, N).rgb;
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 diffuse = irradiance * albedo;

    // indirect specular part

    // prefiltering roughness on LOD
    vec3 prefiltered_color = textureLod(prefilterSampler, R, roughness * prefilter_levels).rgb;

    // BRDF convolution
    vec2 brdf = texture(brdfConvolutionSampler, vec2(NdotV, roughness)).rg;
    vec3 specular = prefiltered_color * (F * brdf.x + brdf.y);

    // ambient part
    return kD * diffuse + specular;
}