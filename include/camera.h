#pragma once

#include <primitives.h>

#include <shader.h>

#include <glm/glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace gl {

    struct ortho_data final {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        float zNear = 0.1f;
        float zFar = 100.0f;
    };

    glm::mat4 ortho_mat(const ortho_data& data);

    struct perspective_data final {
        float fov_degree = 45.0f;
        float aspect_ratio = 1.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;
    };

    glm::mat4 perspective_mat(const perspective_data& data);

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
        float aspect = 1.3;
        float z_near = 0.1f;
        float z_far = 100.0f;
    };

    glm::mat4 view_mat(const camera& data);

    camera camera_init(u32 shader);

    void camera_look(camera& camera, double x, double y);

    void camera_zoom(u32 shader, camera& camera, double x, double y);

    void camera_resize(u32 shader, camera& camera, int w, int h);

    void camera_view_update(u32 shader, camera& camera);
    void camera_perspective_update(u32 shader, camera& camera);

}