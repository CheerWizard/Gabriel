#include <control/camera.h>

namespace gl {

    Camera::Camera(u32 binding, Window* window) : mWindow(window) {
        mUbo.init(binding, 2 * sizeof(glm::mat4));
        update();
    }

    Camera::~Camera() {
        mUbo.free();
    }

    void Camera::look(double x, double y) {
        if (!enableLook || !mWindow->isMousePress(GLFW_MOUSE_BUTTON_LEFT)) return;

        if (mFirstCameraLook) {
            mLastCursorX = (float)x;
            mLastCursorY = (float)y;
            mFirstCameraLook = false;
        }

        float xoffset = (float)x - mLastCursorX;
        float yoffset = mLastCursorY - (float)y;

        mLastCursorX = (float)x;
        mLastCursorY = (float)y;

        xoffset *= horizontalSensitivity;
        yoffset *= verticalSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        clamp(pitch, -maxPitch, maxPitch);

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }

    void Camera::zoom(double x, double y) {
        if (!enableZoom) return;

        fov -= (float) y;
        clamp(fov, 1.0f, maxFov);
        updatePerspective();
    }

    void Camera::resize(int w, int h) {
        updatePerspective();
    }

    void Camera::update() {
        updatePerspective();
        updateView();
    }

    void Camera::updateView() {
        glm::mat4 viewMat = view();
        mUbo.update(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMat));
    }

    glm::mat4 Camera::view() const {
        return getViewMat().init();
    }

    void Camera::updatePerspective() {
        glm::mat4 perspectiveMat = perspective();
        mUbo.update(0, sizeof(glm::mat4), glm::value_ptr(perspectiveMat));
    }

    glm::mat4 Camera::perspective() const {
        return getPerspectiveMat().init();
    }

    void Camera::move(Window* window, float dt) {
        if (!enableMove) return;

        float camera_speed = moveSpeed / dt;

        if (window->isKeyPress(keyMoveForward)) {
            position += camera_speed * front;
        }

        else if (window->isKeyPress(keyMoveLeft)) {
            position -= glm::normalize(glm::cross(front, up)) * camera_speed;
        }

        else if (window->isKeyPress(keyMoveBackward)) {
            position -= camera_speed * front;
        }

        else if (window->isKeyPress(keyMoveRight)) {
            position += glm::normalize(glm::cross(front, up)) * camera_speed;
        }

        updateView();
    }

    RayCollider Camera::shootRay(double x, double y) {
        return { position, raycastWorld(x, y).vec3() };
    }

    ViewRay Camera::raycastView(double x, double y) {
        // mouse Screen -> NDC
        ScreenRay mouseScreenRay((float) x, (float) y, -1.0f, 1.0f);
        NDCRay mouseNdcRay = mouseScreenRay.ndcSpace(mWindow->getFrameWidth(), mWindow->getFrameHeight());
        // mouse NDC -> Clip
        // ignore inverse perspective division, since mouse NDC ray is a vector without depth length
        ClipRay mouseClipRay(mouseNdcRay.x(), mouseNdcRay.y(), -1, 1);
        // mouse Clip -> View
        // z=-1 - forward; w=0 - vector
        ViewRay mouseViewRay = mouseClipRay.viewSpace(perspective());
        mouseViewRay = {mouseViewRay.x(), mouseViewRay.y(), -1, 0 };
        return mouseViewRay;
    }

    WorldRay Camera::raycastWorld(double x, double y) {
        return raycastView(x, y).worldSpace(view());
    }

}