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

    glm::mat4 Camera::view() {
        return glm::lookAt(position, position + front, up);
    }

    void Camera::init(u32 binding, int screen_width, int screen_height) {
        this->screen_width = screen_width;
        this->screen_height = screen_height;
        ubo.init(binding, 2 * sizeof(glm::mat4));
        update();
    }

    void Camera::free() {
        ubo.free();
    }

    void Camera::look(double x, double y) {
        if (!enable_look) return;

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
        if (!enable_zoom) return;

        fov -= (float) y;
        clamp(fov, 1.0f, max_fov);
        update_perspective();
    }

    void Camera::resize(int w, int h) {
        screen_width = w;
        screen_height = h;
        update_perspective();
    }

    void Camera::update_view() {
        glm::mat4 view_mat = view();
        ubo.update({ sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view_mat) });
    }

    void Camera::update_perspective() {
        glm::mat4 perspective_mat = PerspectiveMat({fov, aspect_ratio(), z_near, z_far}).init();
        ubo.update({ 0, sizeof(glm::mat4), glm::value_ptr(perspective_mat) });
    }

    void Camera::update() {
        update_perspective();
        update_view();
    }

    glm::mat4 Camera::perspective() {
        return PerspectiveMat({ fov, aspect_ratio(), z_near, z_far }).init();
    }

    void Camera::move(float dt) {
        if (!enable_move) return;

        float camera_speed = move_speed / dt;

        if (Window::is_key_press(key_move_forward)) {
            position += camera_speed * front;
        }

        else if (Window::is_key_press(key_move_left)) {
            position -= glm::normalize(glm::cross(front, up)) * camera_speed;
        }

        else if (Window::is_key_press(key_move_backward)) {
            position -= camera_speed * front;
        }

        else if (Window::is_key_press(key_move_right)) {
            position += glm::normalize(glm::cross(front, up)) * camera_speed;
        }

        update_view();
    }

    RayCollider Camera::shoot_ray(double x, double y) {
        return { position, raycast_world(x, y).vec3() };
    }

    ViewRay Camera::raycast_view(double x, double y) {
        // mouse Screen -> NDC
        ScreenRay mouse_screen_ray((float) x, (float) y, -1.0f, 1.0f);
        NDCRay mouse_ndc_ray = mouse_screen_ray.ndc_space(screen_width, screen_height);
        // mouse NDC -> Clip
        // ignore inverse perspective division, since mouse NDC ray is a vector without depth length
        ClipRay mouse_clip_ray(mouse_ndc_ray.x(), mouse_ndc_ray.y(), -1, 1);
        // mouse Clip -> View
        // z=-1 - forward; w=0 - vector
        ViewRay mouse_view_ray = mouse_clip_ray.view_space(perspective());
        mouse_view_ray = { mouse_view_ray.x(), mouse_view_ray.y(), -1, 0 };
        return mouse_view_ray;
    }

    WorldRay Camera::raycast_world(double x, double y) {
        return raycast_view(x, y).world_space(view());
    }

}