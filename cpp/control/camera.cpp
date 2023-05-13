#include <control/camera.h>

namespace gl {

    Camera::Camera(const u32 binding, Window* window) : mWindow(window) {
        mUbo.init(binding, 2 * sizeof(glm::mat4));
        update();
    }

    Camera::~Camera() {
        mUbo.free();
    }

    void Camera::look(const double x, const double y) {
        if (!enableLook || !mWindow->isMousePress(GLFW_MOUSE_BUTTON_RIGHT)) {
            mCurrentCursorMode = CursorMode::NORMAL;
            mWindow->setCursorMode(CursorMode::NORMAL);
            return;
        }

        float fx = (float)x;
        float fy = (float)y;

        if (mCurrentCursorMode != CursorMode::DISABLED) {
            mCurrentCursorMode = CursorMode::DISABLED;
            mWindow->setCursorMode(CursorMode::DISABLED);
        }

        if (mFirstCameraLook) {
            mLastCursorX = fx;
            mLastCursorY = fy;
            mFirstCameraLook = false;
        }

        float xoffset = fx - mLastCursorX;
        float yoffset = mLastCursorY - fy;

        mLastCursorX = fx;
        mLastCursorY = fy;

        xoffset *= horizontalSensitivity;
        yoffset *= verticalSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }

    void Camera::zoom(const double y) {
        if (!enableZoom) return;

        mZoomedFOV -= (float) y;
        clamp(mZoomedFOV, 1.0f, fov);
        glm::mat4 perspectiveMat = PerspectiveMat { mZoomedFOV, getAspectRatio(), zNear, zFar }.init();
        mUbo.update(0, sizeof(glm::mat4), glm::value_ptr(perspectiveMat));
    }

    void Camera::resize() {
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

    RayCollider Camera::shootRay(const double x, const double y) {
        return { position, raycastWorld(x, y).vec3() };
    }

    ViewRay Camera::raycastView(const double x, const double y) {
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

    WorldRay Camera::raycastWorld(const double x, const double y) {
        return raycastView(x, y).worldSpace(view());
    }

}