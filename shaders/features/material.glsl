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
uniform Material material;