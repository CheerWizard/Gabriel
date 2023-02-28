#version 420 core

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 bright_color;

in vec3 f_pos;
in vec2 f_uv;
in vec4 direct_light_space_pos;

in mat3 TBN;
in vec3 tangent_pos;
in vec3 tangent_normal;
in vec3 tangent_view_pos;

vec2 UV;
vec3 N;

struct LightPhong {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float refraction;
};

struct LightDirectional {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float refraction;
};

struct LightPoint {
    vec4 position; // vec3 position + float constant
    vec4 ambient; // vec3 ambient + float linear
    vec4 diffuse; // vec3 diffuse + float quadratic
    vec4 specular; // vec3 specular + float refraction
};

struct LightSpot {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float refraction;
};

struct Material {
    // base color
    vec4 color;
    // diffuse
    float diffuse_factor;
    sampler2D diffuse;
    bool enable_diffuse;
    // specularity
    float specular_factor;
    sampler2D specular;
    bool enable_specular;
    float shininess;
    // environment
    samplerCube skybox;
    bool enable_skybox;
    float reflection;
    float refraction;
    // bumping
    sampler2D normal;
    bool enable_normal;
    // parallax
    sampler2D parallax;
    bool enable_parallax;
    float height_scale;
    float parallax_min_layers;
    float parallax_max_layers;
};

layout (std140, binding = 1) uniform Sunlight {
    LightPoint sunlight;
};

layout (std140, binding = 2) uniform Lights {
    LightPoint light_points[4];
};

uniform Material material;

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

vec3 diffuse_function(vec3 light_dir, vec3 light_ambient, vec3 light_diffuse, vec3 material_diffuse)
{
    vec3 ambience = material.color.rgb * light_ambient * material_diffuse;
    float diff_factor = max(dot(N, light_dir), 0);
    vec3 diffuse = material.diffuse_factor * diff_factor * light_diffuse * material_diffuse;
    return ambience + diffuse;
}

vec3 specular_function(vec3 light_dir, vec3 light_specular, vec3 material_specular)
{
    vec3 view_dir = normalize(tangent_view_pos - tangent_pos);
    vec3 H = normalize(view_dir + light_dir);
    float spec_factor = pow(max(dot(N, H), 0), material.shininess);
    return material.specular_factor * spec_factor * light_specular * material_specular;
}

float attenuation_function(vec3 light_position, float q, float l, float c) {
    float d = length(light_position - tangent_pos);
    return 1.0 / ( q * d * d + l * d + c );
}

const float near = 0.1;
const float far  = 100.0;

float linear_depth(float depth) {
    // convert to NDC and then apply reversed non-linear equation
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 reflection_function() {
    vec3 I = normalize(tangent_pos - tangent_view_pos);
    vec3 R = reflect(I, N) * material.reflection;
    return texture(material.skybox, R).rgb;
}

vec3 refraction_function(float light_refraction) {
    float ratio = light_refraction / material.refraction;
    vec3 I = normalize(tangent_pos - tangent_view_pos);
    vec3 R = refract(I, N, ratio);
    return texture(material.skybox, R).rgb;
}

float direct_shadow_function(vec3 light_dir) {
    vec3 proj_coords = direct_light_space_pos.xyz / direct_light_space_pos.w;
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

float point_shadow_function(vec3 light_pos) {
    vec3 fragment_dir = tangent_pos - light_pos;
    float shadow = 0.0;
    float bias = 0.15;
    int samples = sample_offset_directions.length();
    float view_distance = length(tangent_view_pos - tangent_pos);
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

vec2 parallax_function(vec2 uv, vec3 view_dir) {
    float height_scale = material.height_scale;
    float min_layers = material.parallax_min_layers;
    float max_layers = material.parallax_max_layers;
    float layers = mix(max_layers, min_layers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));
    // calculate the size of each layer
    float layer_depth = 1.0 / layers;
    // depth of current layer
    float current_layer_depth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = view_dir.xy / view_dir.z * height_scale;
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

void main()
{
    // parallax mapping
    if (material.enable_parallax) {
        UV = parallax_function(f_uv, normalize(tangent_view_pos - tangent_pos));
        if (UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0)
            discard;
    } else {
        UV = f_uv;
    }

    // normal mapping
    if (material.enable_normal) {
        N = texture(material.normal, UV).xyz;
        N = N * 2.0 - 1.0;
    } else {
        N = normalize(tangent_normal);
    }

    // other mappings
    vec4 material_diffuse = vec4(0.5, 0.5, 0.5, 1.0);
    if (material.enable_diffuse) {
        material_diffuse = texture(material.diffuse, UV);
    }

    vec3 material_specular = vec3(1.0, 1.0, 1.0);
    if (material.enable_specular) {
        material_specular = texture(material.specular, UV).rgb;
    }

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // sunlight
    LightPoint light_point = sunlight;
    vec3 light_point_pos = TBN * light_point.position.xyz;
    vec3 light_point_dir = normalize(light_point_pos - tangent_pos);
    float constant = light_point.position.w;
    float linear = light_point.ambient.w;
    float quadratic = light_point.diffuse.w;
    float refraction = light_point.specular.w;
    float A = attenuation_function(light_point_pos, quadratic, linear, constant);
    float light_point_shadow = point_shadow_function(light_point_pos);
    diffuse += diffuse_function(light_point_dir, light_point.ambient.rgb, light_point.diffuse.rgb, material_diffuse.rgb) * A * (1.0 - light_point_shadow);
    specular += specular_function(light_point_dir, light_point.specular.rgb, material_specular) * A * (1.0 - light_point_shadow);

    // point lights
    for (int i = 0 ; i < light_points.length() ; i++) {
        light_point = light_points[i];
        light_point_pos = TBN * light_point.position.xyz;
        light_point_dir = normalize(light_point_pos - tangent_pos);
        constant = light_point.position.w;
        linear = light_point.ambient.w;
        quadratic = light_point.diffuse.w;
        refraction = light_point.specular.w;
        A = attenuation_function(light_point_pos, quadratic, linear, constant);
        light_point_shadow = point_shadow_function(light_point_pos);
        diffuse += diffuse_function(light_point_dir, light_point.ambient.rgb, light_point.diffuse.rgb, material_diffuse.rgb) * A * (1.0 - light_point_shadow);
        specular += specular_function(light_point_dir, light_point.specular.rgb, material_specular) * A * (1.0 - light_point_shadow);
    }

//    vec3 phong_light_dir = normalize(tangent_light_phong_pos - tangent_pos);
//    float phong_shadow = point_shadow_function(tangent_light_phong_pos);
//
//    diffuse += diffuse_function(phong_light_dir, light_phong.ambient, light_phong.diffuse, material_diffuse.rgb) * (1.0 - phong_shadow);
//    specular += specular_function(phong_light_dir, light_phong.specular, material_specular) * (1.0 - phong_shadow);

//    float direct_shadow = direct_shadow_function(tangent_light_direct_dir);
//    diffuse += diffuse_function(tangent_light_direct_dir, light_directional.ambient, light_directional.diffuse, material_diffuse.rgb) * (1.0 - direct_shadow);
//    specular += specular_function(tangent_light_direct_dir, light_directional.specular, material_specular) * (1.0 - direct_shadow);

//    vec3 spot_light_dir = normalize(tangent_light_spot_pos - tangent_pos);
//    vec3 spot_dir       = normalize(-tangent_light_spot_dir);
//    float theta         = dot(spot_light_dir, spot_dir);
//    float gamma         = light_spot.outer_cutoff;
//    float epsilon       = light_spot.cutoff - gamma;
//    float I             = clamp((theta - gamma) / epsilon, 0.0, 1.0);
//    float spot_shadow = direct_shadow_function(spot_light_dir);
//    diffuse += diffuse_function(spot_light_dir, light_spot.ambient, light_spot.diffuse, material_diffuse.rgb) * I;
//    specular += specular_function(spot_light_dir, light_spot.specular, material_specular) * I;

//    if (material.enable_skybox) {
//        vec3 reflection = reflection_function();
//        vec3 phong_refraction = refraction_function(light_phong.refraction);
//        vec3 directional_refraction = refraction_function(light_directional.refraction);
//        vec3 point_refraction = refraction_function(light_point.refraction);
//        vec3 spot_refraction = refraction_function(light_spot.refraction);
//        vec3 refraction = spot_refraction;
//    }

    vec3 final_color = diffuse.rgb + specular;

    frag_color = vec4(final_color, material.color.a * material_diffuse.a);

    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        bright_color = vec4(frag_color.rgb, 1.0);
    } else {
        bright_color = vec4(0, 0, 0, 1);
    }

//    fragment = vec4(fragment_color, 1.0);
//    fragment = vec4(vec3(linear_depth(gl_FragCoord.z) / far), 1.0);
}