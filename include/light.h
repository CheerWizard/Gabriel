#pragma once

#include <cube.h>
#include <transform.h>
#include <camera.h>
#include <frame.h>

namespace gl {

    struct LightPresent final {
        u32 id = 0;
        glm::vec4 color = { 1, 1, 1, 1 };
        Transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };
        CubeDefault presentation;

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
        float constant = 0;
        float linear = 0;
        float quadratic = 1;
        float refraction = 1;
    };

    struct SpotLight final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float cutoff = glm::cos(glm::radians(6.5f));
        float outer = glm::cos(glm::radians(11.5f));
        float refraction = 1;
        float pad = 0;
    };

}