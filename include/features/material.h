#pragma once

#include <api/shader.h>
#include <ecs/component.h>

namespace gl {

    struct RefractionIndex final {
        constexpr static float AIR = 1.0f;
        constexpr static float WATER = 1.33f;
        constexpr static float ICE = 1.309f;
        constexpr static float GLASS = 1.52f;
        constexpr static float DIAMOND = 2.42f;
    };

    component(Material) {
        static const int slots = 6;
        // base color
        glm::vec4 color = { 1, 1, 1, 1 };
        ImageBuffer albedo;
        bool enableAlbedo = false;
        // bumping
        ImageBuffer normal;
        bool enableNormal = false;
        // parallax
        ImageBuffer parallax;
        bool enableParallax = false;
        float heightScale = 0.1;
        float parallaxMinLayers = 8;
        float parallaxMaxLayers = 32;
        // metallic
        float metallicFactor = 0.5f;
        ImageBuffer metallic;
        bool enableMetallic = false;
        // roughness
        float roughnessFactor = 0.5f;
        ImageBuffer roughness;
        bool enableRoughness = false;
        // AO
        float aoFactor = 0.5f;
        ImageBuffer ao;
        bool enableAO = false;

        Material() = default;

        void init(
            bool flipUV = false,
            const char* albedoPath = null,
            const char* normalPath = null,
            const char* parallaxPath = null,
            const char* metallicPath = null,
            const char* roughnessPath = null,
            const char* aoPath = null
        );

        void free();
        static void free(std::vector<Material>& materials);

        void update(Shader& shader, int slot);
    };

}
