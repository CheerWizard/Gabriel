#include <control/camera.h>

#ifdef IMGUI
#include <imgui.h>
#endif

namespace gl {

    Camera::Camera(const u32 binding, Window* window) : mWindow(window) {
        mUbo.init(binding, sizeof(CameraUniform));
        update();
    }

    Camera::~Camera() {
        mUbo.free();
    }

    void Camera::onMouseCursor(const double x, const double y, const float dt) {
        if (!enableLook || !mWindow->isMousePress(GLFW_MOUSE_BUTTON_RIGHT)) {
            mWindow->setCursorMode(CursorMode::NORMAL);
            return;
        }

        mWindow->setCursorMode(CursorMode::DISABLED);
        float fx = (float)x;
        float fy = (float)y;

        if (mFirstCameraLook) {
            mLastCursorX = fx;
            mLastCursorY = fy;
            mFirstCameraLook = false;
        }

        float xoffset = fx - mLastCursorX;
        float yoffset = mLastCursorY - fy;
        mLastCursorX = fx;
        mLastCursorY = fy;

        if (xoffset != 0 || yoffset != 0) {
            float pitchDt = yoffset * verticalSensitivity * 0.01f / dt;
            float yawDt = xoffset * horizontalSensitivity * 0.01f / dt;
            glm::vec3 right = glm::cross(front, up);
            glm::quat q = glm::normalize(glm::cross(glm::angleAxis(pitchDt, right), glm::angleAxis(-yawDt, up)));
            front = glm::rotate(q, front);
            updateView();
        }
    }

    void Camera::onMouseCursorImgui(const float x, const float y, const float dt) {
#ifdef IMGUI

        if (!enableLook || !ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            mWindow->setCursorMode(CursorMode::NORMAL);
            return;
        }

        mWindow->setCursorMode(CursorMode::DISABLED);
        float fx = x;
        float fy = y;

        if (mFirstCameraLook) {
            mLastCursorX = fx;
            mLastCursorY = fy;
            mFirstCameraLook = false;
        }

        float xoffset = fx - mLastCursorX;
        float yoffset = mLastCursorY - fy;
        mLastCursorX = fx;
        mLastCursorY = fy;

        if (xoffset != 0 || yoffset != 0) {
            float pitchDt = yoffset * verticalSensitivity * 0.01f / dt;
            float yawDt = xoffset * horizontalSensitivity * 0.01f / dt;
            glm::vec3 right = glm::cross(front, up);
            glm::quat q = glm::normalize(glm::cross(glm::angleAxis(pitchDt, right), glm::angleAxis(-yawDt, up)));
            front = glm::rotate(q, front);
            updateView();
        }

#endif
    }

    void Camera::onMouseScroll(const double y, const float dt) {
        if (!enableZoom) {
            mWindow->setCursorMode(CursorMode::NORMAL);
            return;
        }
        mWindow->setCursorMode(CursorMode::DISABLED);

        mZoomedFOV -= (float) y * zoomSpeed / dt;
        clamp(mZoomedFOV, 1.0f, fov);
        glm::mat4 perspectiveMat = PerspectiveMat { mZoomedFOV, getAspectRatio(), zNear, zFar }.init();
        mUbo.update(0, sizeof(glm::mat4), glm::value_ptr(perspectiveMat));
    }

    void Camera::resize() {
        updatePerspective();
    }

    void Camera::update() {
        CameraUniform uniform;
        uniform.perspective = perspective();
        uniform.view = view();
        uniform.position = position;
        mUbo.update(0, sizeof(uniform), &uniform);
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

    void Camera::updatePosition() {
        mUbo.update(sizeof(glm::mat4) * 2, sizeof(glm::vec4), glm::value_ptr(position));
    }

    void Camera::move(const float dt) {
        if (!enableMove) return;

        float distance = moveSpeed / dt;

        if (mWindow->isKeyPress(keyMoveForward)) {
            position += distance * front;
        }

        else if (mWindow->isKeyPress(keyMoveLeft)) {
            position -= glm::normalize(glm::cross(front, up)) * distance;
        }

        else if (mWindow->isKeyPress(keyMoveBackward)) {
            position -= distance * front;
        }

        else if (mWindow->isKeyPress(keyMoveRight)) {
            position += glm::normalize(glm::cross(front, up)) * distance;
        }

        else return;

        updateView();
        updatePosition();
    }

    void Camera::moveImgui(const float dt) {
#ifdef IMGUI

        if (!enableMove) return;

        float distance = moveSpeed / dt;

        if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(keyMoveForward))) {
            position += distance * front;
        }

        else if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(keyMoveLeft))) {
            position -= glm::normalize(glm::cross(front, up)) * distance;
        }

        else if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(keyMoveBackward))) {
            position -= distance * front;
        }

        else if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(keyMoveRight))) {
            position += glm::normalize(glm::cross(front, up)) * distance;
        }

        else return;

        updateView();
        updatePosition();

#endif
    }

    RayCollider Camera::shootRay(const double x, const double y) {
        Ray ray = position;
        ray.direction = { glm::normalize(raycastWorld(x, y).vec3() - position), 0 };
        return ray;
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

    Frustum Camera::frustrum() {
        Frustum frustum;
        const float halfVSide = zFar * tanf(fov * 0.5f);
        const float halfHSide = halfVSide * getAspectRatio();
        const glm::vec3 frontMultFar = zFar * front;
        glm::vec3 right = glm::cross(front, up);
        float positionLength = glm::length(position);

        frustum.nearFace = { glm::length(position + zNear * front), front };
        frustum.farFace = { glm::length(position + frontMultFar), -front };
        frustum.rightFace = { positionLength, glm::cross(frontMultFar - right * halfHSide, up) };
        frustum.leftFace = { positionLength, glm::cross(up, frontMultFar + right * halfHSide) };
        frustum.topFace = { positionLength, glm::cross(right, frontMultFar - up * halfVSide) };
        frustum.bottomFace = { positionLength, glm::cross(frontMultFar + up * halfVSide, right) };

        return frustum;
    }

}