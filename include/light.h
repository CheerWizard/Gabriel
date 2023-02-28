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

        glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        float std140_padding_1 = 0;

        glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
        float std140_padding_2 = 0;

        glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
        float refraction = 1.0f;

        inline float* to_float() const { return (float*) &position.x; }
    };

    struct light_dir final {

        glm::vec3 direction = { -2.0f, 4.0f, -1.0f };
        float std140_padding_0 = 0;

        glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        float std140_padding_1 = 0;

        glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
        float std140_padding_2 = 0;

        glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
        float refraction = 1.0f;

        inline float* to_float() const { return (float*) &direction.x; }
    };

    struct light_point final {

        glm::vec3 position = { 0, 0, 0 };
        float constant = 1.0f;

        glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        float linear = 0.09f;

        glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
        float quadratic = 0.032f;

        glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
        float refraction = 1.0f;

        inline float* to_float() const { return (float*) &position.x; }
    };

    struct light_spot final {
        glm::vec3 position = { 0, 0, 0 };
        float std140_padding_0 = 0;

        glm::vec3 direction = { -0.2f, -1.0f, -0.3f };
        float std140_padding_1 = 0;

        glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        float cutoff = glm::cos(glm::radians(12.5f));

        glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
        float outer = glm::cos(glm::radians(17.5f));

        glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
        float refraction = 1.0f;

        inline float* to_float() const { return (float*) &position.x; }
    };

    void light_present_init(light_present* present);
    void light_present_free();
    void light_present_update();

}