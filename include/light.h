#pragma once

#include <geometry.h>
#include <transform.h>
#include <camera.h>

namespace gl {

    struct light_present final {
        glm::vec4 color = { 1, 1, 1, 1 };
        gl::transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };
        gl::cube_solid presentation;
    };

    struct light_phong final {

        glm::vec3 position = { 0, 0, 0 };
        float std140_padding_0 = 0;

        glm::vec3 color = { 1, 1, 1 };
        float refraction = 1.0f;

        inline float* to_float() const { return (float*) &position.x; }
    };

    struct light_dir final {
        glm::vec4 direction = { -2.0f, 4.0f, -1.0f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct light_point final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float constant = 0.0f;
        float linear = 0.0f;
        float quadratic = 1.0f;
        float refraction = 1.0f;
    };

    struct light_spot final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float cutoff = glm::cos(glm::radians(6.5f));
        float outer = glm::cos(glm::radians(11.5f));
        float refraction = 1.0f;
        float pad = 0;
    };

    void light_present_init(light_present* present);
    void light_present_free();
    void light_present_update();

}