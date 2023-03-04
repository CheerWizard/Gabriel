#pragma once

#include <primitives.h>

#include <shader.h>
#include <buffers.h>

#include <glm/glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace gl {

    struct OrthoMat final {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        float zNear = 0.1f;
        float zFar = 100.0f;

        glm::mat4 init();
    };

    struct PerspectiveMat final {
        float fov_degree = 45.0f;
        float aspect_ratio = 1.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;

        glm::mat4 init();
    };

    struct Camera final {
        glm::vec3 position = { 0, 0, 10 };
        glm::vec3 front = { 0, 0, -1 };
        glm::vec3 up = { 0, 1, 0 };
        float pitch = 0;
        float max_pitch = 89;
        float yaw = 0;
        float roll = 0;
        float speed = 0.5f;
        float sensitivity = 0.05f;
        float fov = 45;
        float max_fov = 45;
        float aspect = 1.3;
        float z_near = 0.1f;
        float z_far = 100.0f;

        UniformBuffer ubo;

        void init(u32 binding, float aspect_ratio);
        glm::mat4 init_view();

        void free();

        void look(double x, double y);

        void zoom(double x, double y);

        void resize(int w, int h);

        void update_view();

        void update_perspective();

        void update();
    };

}