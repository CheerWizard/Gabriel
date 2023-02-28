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
        // diffuse
        float diffuse_factor = 1;
        texture diffuse = { 0, GL_TEXTURE_2D, { "diffuse", 2 } };
        bool enable_diffuse = false;
        // specular
        float specular_factor = 1;
        texture specular = { 0,  GL_TEXTURE_2D, { "specular", 3 } };
        bool enable_specular = false;
        float shininess = 32.0f;
        // environment
        texture skybox = { 0,  GL_TEXTURE_CUBE_MAP, { "skybox", 4 } };
        bool enable_skybox = false;
        float reflection = 1;
        float refraction = 1;
        // bumping
        texture normal = { 0, GL_TEXTURE_2D, { "normal", 5 } };
        bool enable_normal = false;
        // parallax
        texture parallax = { 0, GL_TEXTURE_2D, { "parallax", 6 } };
        bool enable_parallax = false;
        float height_scale = 0.1;
        float parallax_min_layers = 8;
        float parallax_max_layers = 32;
    };

    material material_init(
            const char* diffuse_path = null,
            const char* specular_path = null,
            const char* skybox_path = null,
            const char* normal_path = null,
            const char* parallax_path = null
    );
    void material_free(material& material);

    void material_update_textures(u32 shader, material& material);
    void material_update(u32 shader, material& material);

}
