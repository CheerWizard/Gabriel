uniform sampler2D direct_shadow_sampler;
uniform int shadow_filter_size = 9;
uniform sampler2D shadow_proj_coords;

float direct_shadow_function(vec3 lightDir)
{
    vec3 projCoords = texture(shadow_proj_coords, l_uv).xyz;

    if (projCoords.z > 1.0)
        return 0;

    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(N, lightDir)), 0.005);

    vec2 texelSize = 1.0 / textureSize(direct_shadow_sampler, 0);
    float shadow = 0.0;
    int halfFilterSize = shadow_filter_size / 2;

    for (int y = -halfFilterSize; y <= -halfFilterSize + shadow_filter_size; y++)
    {
        for (int x = -halfFilterSize; x <= -halfFilterSize + shadow_filter_size; x++)
        {
            float pcf_depth = texture(direct_shadow_sampler, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= float(pow(shadow_filter_size, 2));

    return shadow;
}
