#pragma once

#include <shader.h>
#include <component.h>

namespace gl {

    struct RefractionIndex final {
        constexpr static float air = 1.0f;
        constexpr static float water = 1.33f;
        constexpr static float ice = 1.309f;
        constexpr static float glass = 1.52f;
        constexpr static float diamond = 2.42f;
    };

    component(Material) {
        static const int slots = 6;
        // base color
        glm::vec4 color = { 1, 1, 1, 1 };
        ImageBuffer albedo;
        bool enable_albedo = false;
        // bumping
        ImageBuffer normal;
        bool enable_normal = false;
        // parallax
        ImageBuffer parallax;
        bool enable_parallax = false;
        float height_scale = 0.1;
        float parallax_min_layers = 8;
        float parallax_max_layers = 32;
        // metallic
        float metallic_factor = 0.5f;
        ImageBuffer metallic;
        bool enable_metallic = false;
        // roughness
        float roughness_factor = 0.5f;
        ImageBuffer roughness;
        bool enable_roughness = false;
        // AO
        float ao_factor = 0.5f;
        ImageBuffer ao;
        bool enable_ao = false;

        Material() = default;

        void init(
            bool flip_uv = false,
            const char* albedo_path = null,
            const char* normal_path = null,
            const char* parallax_path = null,
            const char* metallic_path = null,
            const char* roughness_path = null,
            const char* ao_path = null
        );

        void free();
        static void free(std::vector<Material>& materials);

        void update(Shader& shader, int slot);
    };

}
