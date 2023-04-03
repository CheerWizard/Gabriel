#include <camera.h>
#include <math_functions.h>
#include <window.h>
#include <buffers.h>

namespace gl {

    static float last_cursor_x = 400;
    static float last_cursor_y = 300;
    static bool first_camera_look = true;

    glm::mat4 OrthoMat::init() {
        return glm::ortho(left, right, bottom, top, zNear, zFar);
    }

    glm::mat4 PerspectiveMat::init() {
        return glm::perspective(glm::radians(fov_degree), aspect_ratio, zNear, zFar);
    }

    glm::mat4 Camera::init_view() {
        return glm::lookAt(position, position + front, up);
    }

    void Camera::init(u32 binding, float aspect_ratio) {
        aspect = win::get_aspect_ratio();
        ubo.init(binding, 2 * sizeof(glm::mat4));
        update();
    }

    void Camera::free() {
        ubo.free();
    }

    void Camera::look(double x, double y) {
        if (first_camera_look) {
            last_cursor_x = (float)x;
            last_cursor_y = (float)y;
            first_camera_look = false;
        }

        float xoffset = (float)x - last_cursor_x;
        float yoffset = last_cursor_y - (float)y;

        last_cursor_x = (float)x;
        last_cursor_y = (float)y;

        xoffset *= horizontal_sensitivity;
        yoffset *= vertical_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        clamp(pitch, -max_pitch, max_pitch);

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }

    void Camera::zoom(double x, double y) {
        fov -= (float) y;
        clamp(fov, 1.0f, max_fov);
        update_perspective();
    }

    void Camera::resize(int w, int h) {
        aspect = (float)w / (float)h;
        update_perspective();
    }

    void Camera::update_view() {
        glm::mat4 view = init_view();
        ubo.update({ sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view) });
    }

    void Camera::update_perspective() {
        glm::mat4 perspective = PerspectiveMat({fov, aspect, z_near, z_far}).init();
        ubo.update({ 0, sizeof(glm::mat4), glm::value_ptr(perspective) });
    }

    void Camera::update() {
        update_perspective();
        update_view();
    }

    glm::mat4 Camera::perspective() {
        return PerspectiveMat({ fov, aspect, z_near, z_far }).init();
    }

}