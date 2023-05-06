#pragma once

#include <core/common.h>
#include <core/window.h>
#include <core/keycodes.h>

#include <api/shader.h>
#include <api/buffers.h>

#include <math/matrices.h>
#include <math/collisions.h>

namespace gl {

    struct Camera final {
        glm::ivec2 resolution = { 800, 600 };
        glm::vec3 position = { 0, 0, 10 };
        glm::vec3 front = { 0, 0, -1 };
        glm::vec3 up = { 0, 1, 0 };
        float pitch = 0;
        float maxPitch = 89;
        float yaw = 0;
        float roll = 0;
        float moveSpeed = 1.0f;
        float horizontalSensitivity = 0.15f;
        float verticalSensitivity = 0.30f;
        float fov = 45;
        float maxFov = 45;
        float zNear = 0.1f;
        float zFar = 100.0f;

        bool enableLook = true;
        bool enableZoom = true;
        bool enableMove = true;

        KEY keyMoveForward = KEY::W;
        KEY keyMoveLeft = KEY::A;
        KEY keyMoveBackward = KEY::S;
        KEY keyMoveRight = KEY::D;

        void init(u32 binding, int w, int h);
        void free();

        void look(double x, double y);
        void zoom(double x, double y);
        void move(Window* window, float dt);

        void resize(int w, int h);

        glm::mat4 perspective() const;
        void updatePerspective();
        [[nodiscard]] inline PerspectiveMat getPerspectiveMat() const {
            return PerspectiveMat {fov, getAspectRatio(), zNear, zFar };
        }

        glm::mat4 view() const;
        void updateView();
        [[nodiscard]] inline ViewMat getViewMat() const {
            return ViewMat { position, position + front, up };
        }

        void update();

        inline float getAspectRatio() const {
            return (float) resolution.x / (float) resolution.y;
        }

        RayCollider shootRay(double x, double y);
        ViewRay raycastView(double x, double y);
        WorldRay raycastWorld(double x, double y);

    private:
        UniformBuffer mUbo;
        float mLastCursorX = 400;
        float mLastCursorY = 300;
        bool mFirstCameraLook = true;
    };

}