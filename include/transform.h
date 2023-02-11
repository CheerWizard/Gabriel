#pragma once

#include <primitives.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace gl {

    struct transform final {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::fvec3 scale = { 1, 1, 1 };
    };

    glm::mat4 mat_model(const transform& data);

    void shader_set_model(u32 shader, const transform& data);

    struct camera final {
        glm::vec3 position = { 0, 0, 3 };
        glm::vec3 front = { 0, 0, -1 };
        glm::vec3 up = { 0, 1, 0 };
        float pitch = 0;
        float yaw = 0;
        float roll = 0;
        float speed = 0.5f;
        float sensitivity = 0.05f;
        float fov = 45;
        float max_fov = 45;
    };

    glm::mat4 mat_view(const camera& data);

    void shader_set_camera(u32 shader, const camera& camera);

    struct ortho_data final {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        float zNear = 0.1f;
        float zFar = 100.0f;
    };

    glm::mat4 mat_ortho(const ortho_data& data);

    struct perspective_data final {
        float fov_degree = 45.0f;
        float aspect_ratio = 1.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;
    };

    glm::mat4 mat_perspective(const perspective_data& data);

}