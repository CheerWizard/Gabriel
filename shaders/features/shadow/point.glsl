uniform samplerCube point_shadow_sampler;
uniform float far_plane;

vec3 sample_offset_directions[20] = vec3[]
(
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float point_shadow_function(vec3 lightPos)
{
    vec3 fragmentDir = w_pos - lightPos;
    float shadow = 0.0;
    float bias = 0.15;
    int samples = sample_offset_directions.length();
    float viewDistance = length(camera_pos - w_pos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    float currentDepth = length(fragmentDir);

    for (int i = 0; i < samples; i++) {
        float closestDepth = texture(point_shadow_sampler, fragmentDir + sample_offset_directions[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
        shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}
