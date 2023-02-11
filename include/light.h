#pragma once

#include <geometry.h>
#include <transform.h>
#include <camera.h>

namespace gl {

    struct light final {
        glm::vec4 color = { 1, 1, 1, 1 };

        glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
        glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
        glm::vec3 specular = { 1.0f, 1.0f, 1.0f };

        gl::transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };

        gl::cube_solid presentation;
    };

    void light_init(u32 shader, camera& camera, light& light);
    void light_free();
    void light_update(u32 shader, camera& camera, light& light);

}