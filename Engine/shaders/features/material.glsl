struct Material {
    // base color
    sampler2D albedo;
    bool enable_albedo;
    vec4 color;
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
    sampler2D metallic;
    bool enable_metallic;
    float metallic_factor;
    // roughness
    sampler2D roughness;
    bool enable_roughness;
    float roughness_factor;
    // AO
    sampler2D ao;
    bool enable_ao;
    float ao_factor;
    // Emission
    sampler2D emission;
    bool enable_emission;
    vec3 emission_factor;
};
uniform Material material;
