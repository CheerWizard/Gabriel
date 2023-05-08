#version 460 core

in vec2 l_uv;

layout(location = 0) out vec4 out_color;

vec3 w_pos;
vec3 N;
vec3 V;
vec3 R;

const float PI = 3.14159265359;

struct LightPhong {
    vec3 position;
    vec4 color;
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

uniform int shadow_filter_size = 9;

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D albedos;
uniform sampler2D pbr_params;
uniform sampler2D shadow_proj_coords;

uniform sampler2D ssao;
uniform bool enable_ssao;

float attenuation_function(vec3 light_position, float q, float l, float c)
{
    float d = length(light_position - w_pos);
    return 1.0 / ( q * d * d + l * d + c );
}

float direct_shadow_function(vec3 light_dir)
{
    vec3 proj_coords = texture(shadow_proj_coords, l_uv).xyz;

    if (proj_coords.z > 1.0)
        return 0;

    float current_depth = proj_coords.z;

    float bias = max(0.05 * (1.0 - dot(N, light_dir)), 0.005);

    vec2 texel_size = 1.0 / textureSize(direct_shadow_sampler, 0);
    float shadow = 0.0;
    int halfFilterSize = shadow_filter_size / 2;

    for (int y = -halfFilterSize; y <= -halfFilterSize + shadow_filter_size; y++)
    {
        for (int x = -halfFilterSize; x <= -halfFilterSize + shadow_filter_size; x++)
        {
            float pcf_depth = texture(direct_shadow_sampler, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= float(pow(shadow_filter_size, 2));

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
    vec3 tangent_light_pos = light_point.position;
    vec3 light_dir = normalize(tangent_light_pos - w_pos);
    vec3 light_color = light_point.color;
    float constant  = light_point.constant;
    float linear    = light_point.linear;
    float quadratic = light_point.quadratic;
    float A = attenuation_function(tangent_light_pos, quadratic, linear, constant);
    float light_point_shadow = point_shadow_function(tangent_light_pos);
    float radiance_factor = A * (1.0 - light_point_shadow);

    return pbr(light_dir, light_color, radiance_factor, albedo, metallic, roughness);
}

vec3 pbr(LightSpot light_spot, vec3 albedo, float metallic, float roughness)
{
    vec3 tangent_light_pos = light_spot.position;
    vec3 tangent_spot_dir = light_spot.direction;
    vec3 light_dir = normalize(tangent_light_pos - w_pos);
    vec3 light_color = light_spot.color;
    vec3 spot_dir       = normalize(-tangent_spot_dir);
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
    vec4 w_positions = texture(positions, l_uv);
    vec4 w_normals = texture(normals, l_uv);
    vec4 albedo = texture(albedos, l_uv);
    vec4 pbr_param = texture(pbr_params, l_uv);
    vec4 w_shadow_proj_coords = texture(shadow_proj_coords, l_uv);

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

    // PBR sunlight
    Lo += pbr(sunlight, albedo.rgb, metallic, roughness);

    // PBR multiple point lights
    int light_points_size = light_points.length();
    for (int i = 0 ; i < light_points_size ; i++) {
        Lo += pbr(light_points[i], albedo.rgb, metallic, roughness);
    }

    // PBR flashlight
    Lo += pbr(flashlight, albedo.rgb, metallic, roughness);

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

    // ambient part
    vec3 ambient = kD * diffuse + specular;

    out_color = vec4((ambient + Lo) * ao, albedo.a);
}