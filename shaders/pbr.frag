#version 460 core

in vec2 l_uv;
in vec3 w_pos;
in vec3 w_normal;
in vec4 dls_pos;

layout(location = 0) out vec4 out_color;
layout(location = 1) out float out_reveal;
layout(location = 2) out uint out_entity_id;

vec2 UV;
vec3 N;
vec3 V;
vec3 R;

const float PI = 3.14159265359;

struct LightPhong {
    vec3 position; // vec3 position
    vec4 color;  // vec3 color + float refraction
};

struct LightDirectional {
    vec3 position;
    vec4 color;
};

struct LightPoint {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float refraction;
};

struct LightSpot {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutoff;
    float outer;
    float refraction;
};

struct LightEnvironmental {
    int prefilter_levels;
    samplerCube irradiance;
    samplerCube prefilter;
    sampler2D brdf_convolution;
};

struct Material {
    // base color
    vec4 color;
    sampler2D albedo;
    bool enable_albedo;
    // bumping
    sampler2D normal;
    bool enable_normal;
    // parallax
    sampler2D parallax;
    bool enable_parallax;
    float height_scale;
    float parallax_min_layers;
    float parallax_max_layers;
    // metalness
    float metallic_factor;
    sampler2D metallic;
    bool enable_metallic;
    // roughness
    float roughness_factor;
    sampler2D roughness;
    bool enable_roughness;
    // AO
    float ao_factor;
    sampler2D ao;
    bool enable_ao;
};

layout (std140, binding = 1) uniform Sunlight {
    LightDirectional sunlight;
};

layout (std140, binding = 2) uniform Lights {
    LightPoint light_points[4];
};

layout (std140, binding = 3) uniform FlashLight {
    LightSpot flashlight;
};

uniform vec3 camera_pos;
uniform LightEnvironmental envlight;

uniform sampler2D direct_shadow_sampler;

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

uniform Material material;

uniform uint entity_id;

float attenuation_function(vec3 light_position, float q, float l, float c)
{
    float d = length(light_position - w_pos);
    return 1.0 / ( q * d * d + l * d + c );
}

float direct_shadow_function(vec3 light_dir)
{
    vec3 proj_coords = dls_pos.xyz / dls_pos.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0)
        return 0;

    float current_depth = proj_coords.z;

    float bias = max(0.05 * (1.0 - dot(N, light_dir)), 0.005);

    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(direct_shadow_sampler, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(direct_shadow_sampler, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

float point_shadow_function(vec3 light_pos)
{
    vec3 fragment_dir = w_pos - light_pos;
    float shadow = 0.0;
    float bias = 0.15;
    int samples = sample_offset_directions.length();
    float view_distance = length(camera_pos - w_pos);
    float disk_radius = (1.0 + (view_distance / far_plane)) / 25.0;
    float current_depth = length(fragment_dir);

    for (int i = 0; i < samples; i++) {
        float closest_depth = texture(point_shadow_sampler, fragment_dir + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= far_plane;   // undo mapping [0;1]
        if (current_depth - bias > closest_depth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec2 parallax_function(vec2 uv)
{
    float height_scale = material.height_scale;
    float min_layers = material.parallax_min_layers;
    float max_layers = material.parallax_max_layers;
    float layers = mix(max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), V)));
    // calculate the size of each layer
    float layer_depth = 1.0 / layers;
    // depth of current layer
    float current_layer_depth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = V.xy / V.z * height_scale;
    vec2 delta_uv = P / layers;

    vec2 current_uv = uv;
    float current_parallax_value = texture(material.parallax, current_uv).r;
    while (current_layer_depth < current_parallax_value) {
        // shift texture coordinates along direction of P
        current_uv -= delta_uv;
        // get depthmap value at current texture coordinates
        current_parallax_value = texture(material.parallax, current_uv).r;
        // get depth of next layer
        current_layer_depth += layer_depth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prev_uv = current_uv + delta_uv;

    // get depth after and before collision for linear interpolation
    float after_depth  = current_parallax_value - current_layer_depth;
    float before_depth = texture(material.parallax, prev_uv).r - current_layer_depth + layer_depth;

    // interpolation of texture coordinates
    float weight = after_depth / (after_depth - before_depth);
    vec2 final_uv = prev_uv * weight + current_uv * (1.0 - weight);

    return final_uv;
}

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

vec3 pbr(vec3 L, vec3 light_color, float radiance_factor, vec3 albedo, float metallic, float roughness)
{
    vec3 H = normalize(V + L);
    vec3 radiance = light_color * radiance_factor;

    // diffuse refraction/specular reflection ratio on surface
    vec3 F0 = vec3(0.04); // base reflection 0.04 covers most of dielectrics
    F0      = mix(F0, albedo, metallic); // F0 = albedo color if it's a complete metal
    vec3 F  = fresnel_shlick(max(dot(H, V), 0), F0);

    // normal distribution halfway vector along rough surface
    float D = distribution_ggx(H, roughness);

    // geometry obstruction and geometry shadowing of microfacets
    float G = geometry_smith(V, L, roughness);

    // BRDF Cook-Torrance approximation
    float y = 0.0001; // infinetely small number used when dot product N/V or N/L returns 0
    vec3 specular = (D * G * F) / ( 4.0 * max(dot(N, V), 0) * max(dot(N, L), 0) + y);

    // light contribution to reflectance equation
    vec3 ks = F;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallic;

    // diffuse absorption/scatter using Lambert function
    vec3 diffuse = lambert(kd, albedo);

    // solve reflectance equation
    return (diffuse + specular) * radiance * max(dot(N, L), 0);
}

vec3 pbr(LightPoint light_point, vec3 albedo, float metallic, float roughness)
{
    vec3 light_pos = light_point.position;
    vec3 light_dir = normalize(light_pos - w_pos);
    vec3 light_color = light_point.color;
    float constant  = light_point.constant;
    float linear    = light_point.linear;
    float quadratic = light_point.quadratic;
    float A = attenuation_function(light_pos, quadratic, linear, constant);
    float light_point_shadow = point_shadow_function(light_pos);
    float radiance_factor = A * (1.0 - light_point_shadow);

    return pbr(light_dir, light_color, radiance_factor, albedo, metallic, roughness);
}

vec3 pbr(LightSpot light_spot, vec3 albedo, float metallic, float roughness)
{
    vec3 light_pos = light_spot.position;
    vec3 light_dir = normalize(light_pos - w_pos);
    vec3 light_color = light_spot.color;
    vec3 spot_dir       = normalize(-light_spot.direction);
    float theta         = dot(light_dir, spot_dir);
    float cutoff        = light_spot.cutoff;
    float gamma         = light_spot.outer;
    float epsilon       = cutoff - gamma;
    float I             = clamp((theta - gamma) / epsilon, 0.0, 1.0);
    float radiance_factor = I * (1.0);

    return pbr(light_dir, light_color, radiance_factor, albedo, metallic, roughness);
}

vec3 pbr(LightDirectional light_direct, vec3 albedo, float metallic, float roughness)
{
    vec3 light_dir = light_direct.position;
    vec3 light_color = light_direct.color.rgb * light_direct.color.a;
    float light_direct_shadow = direct_shadow_function(light_dir);
    float radiance_factor = 1.0 - light_direct_shadow;
    return pbr(light_dir, light_color, radiance_factor, albedo, metallic, roughness);
}

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

    // PBR
    vec3 Lo = vec3(0);

    // PBR sunlight
    Lo += pbr(sunlight, albedo.rgb, metallic, roughness);

    // PBR multiple point lights
//    int light_points_size = light_points.length();
//    for (int i = 0 ; i < light_points_size ; i++) {
//        Lo += pbr(light_points[i], albedo.rgb, metallic, roughness);
//    }

    // PBR flashlight
//    Lo += pbr(flashlight, albedo.rgb, metallic, roughness);

    // PBR env light
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);
    vec3 F = fresnel_shlick_rough(NdotV, F0, roughness);

    // indirect diffuse part
    vec3 irradiance = texture(envlight.irradiance, N).rgb;
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 diffuse = irradiance * albedo.rgb;

    // indirect specular part
    // prefiltering roughness on LOD
    vec3 prefiltered_color = textureLod(envlight.prefilter, R, roughness * envlight.prefilter_levels).rgb;
    // BRDF convolution
    vec2 brdf = texture(envlight.brdf_convolution, vec2(NdotV, roughness)).rg;
    vec3 specular = prefiltered_color * (F * brdf.x + brdf.y);

    // PBR Ambient part
    vec3 ambient = kD * diffuse + specular;

    out_color = vec4((ambient + Lo) * ao, albedo.a);

    // weigth and blend transparent material
//    if (out_color.a < 1) {
//         weight function
//        float weight = clamp(pow(min(1.0, out_color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
//         store pixel revealage threshold
//        out_reveal = out_color.a;
//         store pixel color accumulation
//        out_color = vec4(out_color.rgb * out_color.a, out_color.a) * weight;
//    }

    out_entity_id = entity_id;
}