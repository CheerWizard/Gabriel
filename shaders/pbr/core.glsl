vec3 fresnel_shlick(float cos_theta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

vec3 fresnel_shlick_rough(float cos_theta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float distribution_ggx(vec3 H, float roughness)
{
    float a   = roughness * roughness;
    float a2  = a * a;
    float NH  = max(dot(N, H), 0);
    float NH2 = NH * NH;
    float x = (NH2 * (a2 - 1.0) + 1.0);

    return a2 / (PI * x * x);
}

float geometry_shlick_ggx(float NV, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NV / (NV * (1.0 - k) + k);
}

float geometry_smith(vec3 V, vec3 L, float roughness)
{
    float NV = max(dot(N, V), 0);
    float NL = max(dot(N, L), 0);
    return geometry_shlick_ggx(NV, roughness) * geometry_shlick_ggx(NL, roughness);
}

vec3 lambert(vec3 kd, vec3 albedo)
{
    return kd * albedo / PI;
}
