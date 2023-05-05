#pragma once

#include <primitives.h>

#include <matrices.h>

#include <shader.h>
#include <buffers.h>

#include <window.h>
#include <keycodes.h>

#include <collisions.h>

namespace gl {

    struct Camera final {
        glm::vec3 position = { 0, 0, 10 };
        glm::vec3 front = { 0, 0, -1 };
        glm::vec3 up = { 0, 1, 0 };
        float pitch = 0;
        float max_pitch = 89;
        float yaw = 0;
        float roll = 0;
        float move_speed = 1.0f;
        float horizontal_sensitivity = 0.15f;
        float vertical_sensitivity = 0.30f;
        float fov = 45;
        float max_fov = 45;
        float z_near = 0.1f;
        float z_far = 100.0f;
        int screen_width = 800;
        int screen_height = 600;

        bool enable_look = true;
        bool enable_zoom = true;
        bool enable_move = true;

        KEY key_move_forward = KEY::W;
        KEY key_move_left = KEY::A;
        KEY key_move_backward = KEY::S;
        KEY key_move_right = KEY::D;

        void init(u32 binding, int screen_width, int screen_height);

        glm::mat4 view() const;
        void update_view();

        void free();

        void look(double x, double y);

        void zoom(double x, double y);

        void move(float dt);

        void resize(int w, int h);

        glm::mat4 perspective() const;
        void update_perspective();

        void update();

        inline float aspect_ratio() const {
            return (float) screen_width / (float) screen_height;
        }

        RayCollider shoot_ray(double x, double y);
        ViewRay raycast_view(double x, double y);
        WorldRay raycast_world(double x, double y);

    private:
        UniformBuffer ubo;
    };

}