#pragma once

#include <core/window.h>

#include <api/shader.h>
#include <api/buffers.h>

namespace gl {

    struct GABRIEL_API CameraUniform final {
        glm::mat4 perspective;
        glm::mat4 view;
        glm::vec3 position;
        float pad = 0;
    };

    struct GABRIEL_API Camera final {
        glm::vec3 position = { 0, 0, 10 };
        glm::vec3 front = { 0, 0, -1 };
        glm::vec3 up = { 0, 1, 0 };

        float pitch = 0;
        float yaw = 0;
        float roll = 0;

        float moveSpeed = 1.0f;
        float zoomSpeed = 3.0f;
        float horizontalSensitivity = 1.0f;
        float verticalSensitivity = 1.0f;

        float fov = 45;
        float zNear = 0.1f;
        float zFar = 100.0f;

        bool enableLook = true;
        bool enableZoom = true;
        bool enableMove = true;

        KEY keyMoveForward = KEY::W;
        KEY keyMoveLeft = KEY::A;
        KEY keyMoveBackward = KEY::S;
        KEY keyMoveRight = KEY::D;

        Camera(const u32 binding, Window* window);
        ~Camera();

        void onMouseCursor(const double x, const double y, const float dt);
        void onMouseCursorImgui(const float x, const float y, const float dt);

        void onMouseScroll(const double y, const float dt);

        void move(const float dt);
        void moveImgui(const float dt);

        void resize();

        glm::mat4 perspective() const;
        void updatePerspective();
        [[nodiscard]] inline PerspectiveMat getPerspectiveMat() const {
            return PerspectiveMat { fov, getAspectRatio(), zNear, zFar };
        }

        glm::mat4 view() const;
        void updateView();
        [[nodiscard]] inline ViewMat getViewMat() const {
            return ViewMat { position, position + front, up };
        }

        void updatePosition();

        void update();

        [[nodiscard]] inline float getAspectRatio() const {
            return mWindow->getAspectRatio();
        }

        RayCollider shootRay(const double x, const double y);
        ViewRay raycastView(const double x, const double y);
        WorldRay raycastWorld(const double x, const double y);

        Frustum frustrum();

    private:
        Window* mWindow;
        UniformBuffer mUbo;
        float mLastCursorX = 400;
        float mLastCursorY = 300;
        bool mFirstCameraLook = true;
        float mZoomedFOV = fov;
    };

}