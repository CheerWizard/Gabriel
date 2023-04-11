#version 460 core

in vec2 l_uv;
in vec3 w_pos;
in vec3 w_normal;
in vec4 dls_pos;
in vec3 v_pos;
in vec3 v_normal;

layout(location = 0) out vec4 out_position; // position
layout(location = 1) out vec4 out_normal; // normal
layout(location = 2) out vec4 out_albedo; // albedo + transparency
layout(location = 3) out vec4 out_pbr_params; // metalness + roughness + occlusion
layout(location = 4) out vec4 out_shadow_proj_coords; // shadow proj coords
layout(location = 5) out uint out_entity_id;
layout(location = 6) out vec4 out_view_position;
layout(location = 7) out vec4 out_view_normal;

vec2 UV;
vec3 V;

const float PI = 3.14159265359;

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
    // ambient occlusion
    float ao_factor;
    sampler2D ao;
    bool enable_ao;
};

uniform vec3 camera_pos;

uniform Material material;

uniform uint entity_id;

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

void main()
{
    UV = l_uv;
    V = normalize(camera_pos - w_pos);
    vec3 world_normal = normalize(w_normal);
    vec3 view_normal = normalize(v_normal);

    // parallax mapping
    if (material.enable_parallax) {
        UV = parallax_function(l_uv);
        if (UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0)
            discard;
    }

    // normal mapping
    if (material.enable_normal) {
        vec3 tangentNormal = texture(material.normal, UV).xyz * 2.0 - 1.0;
        {
            vec3 Q1  = dFdx(w_pos);
            vec3 Q2  = dFdy(w_pos);
            vec2 st1 = dFdx(UV);
            vec2 st2 = dFdy(UV);

            vec3 T = normalize(Q1*st2.t - Q2*st1.t);
            vec3 B = -normalize(cross(world_normal, T));
            mat3 TBN = mat3(T, B, world_normal);

            world_normal = normalize(TBN * tangentNormal);
        }
        {
            vec3 Q1  = dFdx(v_pos);
            vec3 Q2  = dFdy(v_pos);
            vec2 st1 = dFdx(UV);
            vec2 st2 = dFdy(UV);

            vec3 T = normalize(Q1*st2.t - Q2*st1.t);
            vec3 B = -normalize(cross(view_normal, T));
            mat3 TBN = mat3(T, B, view_normal);

            view_normal = normalize(TBN * tangentNormal);
        }
    }

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

    out_position = vec4(w_pos, 1.0);
    out_normal = vec4(world_normal, 1.0);
    out_albedo = albedo;
    out_pbr_params = vec4(metallic, roughness, ao, 1.0);
    out_shadow_proj_coords = vec4((dls_pos.xyz / dls_pos.w) * 0.5 + 0.5, 1.0);
    out_view_position = vec4(v_pos, 1.0);
    out_view_normal = vec4(view_normal, 1.0);

    out_entity_id = entity_id;
}