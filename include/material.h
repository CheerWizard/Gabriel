#pragma once

#include <texture.h>

namespace gl {

    struct refraction final {
        constexpr static float air = 1.0f;
        constexpr static float water = 1.33f;
        constexpr static float ice = 1.309f;
        constexpr static float glass = 1.52f;
        constexpr static float diamond = 2.42f;
    };

    struct material final {
        // base color
        glm::vec4 color = { 1, 1, 1, 1 };
        texture albedo = { 0, GL_TEXTURE_2D, { "albedo", 2 } };
        bool enable_albedo = false;
        // bumping
        texture normal = { 0, GL_TEXTURE_2D, { "normal", 3 } };
        bool enable_normal = false;
        // parallax
        texture parallax = { 0, GL_TEXTURE_2D, { "parallax", 4 } };
        bool enable_parallax = false;
        float height_scale = 0.1;
        float parallax_min_layers = 8;
        float parallax_max_layers = 32;
        // metallic
        float metallic_factor = 0.5f;
        texture metallic = { 0, GL_TEXTURE_2D, { "metallic", 5 } };
        bool enable_metallic = false;
        // roughness
        float roughness_factor = 0.5f;
        texture roughness = { 0, GL_TEXTURE_2D, { "roughness", 6 } };
        bool enable_roughness = false;
        // AO
        float ao_factor = 0.5f;
        texture ao = { 0, GL_TEXTURE_2D, { "ao", 7 } };
        bool enable_ao = false;
        // env
        texture env = { 0,  GL_TEXTURE_CUBE_MAP, { "env", 8 } };
        bool enable_env = false;
        float reflection = 1;
        float refraction = 1;
        // env irradiance
        texture env_irradiance = { 0,  GL_TEXTURE_CUBE_MAP, { "env_irradiance", 9 } };
        bool enable_env_irradiance = false;
    };

    material material_init(
            bool flip_uv = false,
            const char* albedo_path = null,
            const char* normal_path = null,
            const char* parallax_path = null,
            const char* metallic_path = null,
            const char* roughness_path = null,
            const char* ao_path = null
    );
    void material_free(material& material);

    void material_update_textures(u32 shader, material& material);
    void material_update(u32 shader, material& material);

}
