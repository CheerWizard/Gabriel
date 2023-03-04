#pragma once

#include <geometry.h>
#include <transform.h>
#include <camera.h>

namespace gl {

    struct LightPresent final {
        glm::vec4 color = { 1, 1, 1, 1 };
        Transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };
        CubeSolid presentation;

        void init();

        void free();

        void update();
    };

    struct PhongLight final {
        glm::vec4 position = { 0, 0, 0, 1 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct DirectLight final {
        glm::vec4 direction = { -2.0f, 4.0f, -1.0f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct PointLight final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
        float refraction = 1.0f;
    };

    struct SpotLight final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float cutoff = glm::cos(glm::radians(6.5f));
        float outer = glm::cos(glm::radians(11.5f));
        float refraction = 1.0f;
        float pad = 0;
    };

    struct EnvLight final {
        int resolution = 2048;
        int prefilter_resolution = 1024;
        int prefilter_levels = 10;
        Texture env = { 0, GL_TEXTURE_CUBE_MAP, { "sampler", 0 } };
        Texture irradiance = { 0, GL_TEXTURE_CUBE_MAP, { "irradiance", 8 } };
        Texture prefilter = { 0, GL_TEXTURE_CUBE_MAP, { "prefilter", 9 } };
        Texture brdf_convolution = { 0, GL_TEXTURE_2D, { "brdf_convolution", 10 } };

        void init();

        void generate(const Texture& hdr_texture);

        void free();

        void update(Shader& shader);

        void render();
    };

}