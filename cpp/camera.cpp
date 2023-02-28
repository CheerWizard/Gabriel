#include <camera.h>
#include <math_functions.h>
#include <window.h>
#include <buffers.h>

namespace gl {

    glm::mat4 ortho_mat(const ortho_data& data) {
        return glm::ortho(data.left, data.right, data.bottom, data.top, data.zNear, data.zFar);
    }

    glm::mat4 perspective_mat(const perspective_data& data) {
        return glm::perspective(glm::radians(data.fov_degree), data.aspect_ratio, data.zNear, data.zFar);
    }

    glm::mat4 view_mat(const camera& data) {
        return glm::lookAt(
                data.position,
                data.position + data.front,
                data.up
        );
    }

    static float s_last_x = 400;
    static float s_last_y = 300;
    static bool s_first_mouse = true;

    static u32 camera_ubo;

    camera camera_init(u32 binding) {
        camera new_camera;
        new_camera.aspect = win::get_aspect_ratio();

        camera_ubo = ubo_init(binding, 2 * sizeof(glm::mat4));

        camera_update(new_camera);

        return new_camera;
    }

    void camera_free() {
        ubo_free(camera_ubo);
    }

    void camera_look(camera& camera, double x, double y) {
        if (s_first_mouse) {
            s_last_x = (float)x;
            s_last_y = (float)y;
            s_first_mouse = false;
        }

        float xoffset = (float)x - s_last_x;
        float yoffset = s_last_y - (float)y;

        s_last_x = (float)x;
        s_last_y = (float)y;

        xoffset *= camera.sensitivity;
        yoffset *= camera.sensitivity;

        camera.yaw += xoffset;
        camera.pitch += yoffset;

        clamp(camera.pitch, -camera.max_pitch, camera.max_pitch);

        glm::vec3 direction;
        direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        direction.y = sin(glm::radians(camera.pitch));
        direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(direction);
    }

    void camera_zoom(camera& camera, double x, double y) {
        camera.fov -= (float) y;
        clamp(camera.fov, 1.0f, camera.max_fov);
        camera_perspective_update(camera);
    }

    void camera_resize(camera& camera, int w, int h) {
        camera.aspect = (float)w / (float)h;
        camera_perspective_update(camera);
    }

    void camera_view_update(camera& camera) {
        glm::mat4 view = view_mat(camera);
        ubo_update(camera_ubo, { sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view) });
    }

    void camera_perspective_update(camera& camera) {
        glm::mat4 perspective = perspective_mat({
            camera.fov,
            camera.aspect,
            camera.z_near,
            camera.z_far
        });
        ubo_update(camera_ubo, { 0, sizeof(glm::mat4), glm::value_ptr(perspective) });
    }

    void camera_update(camera& camera) {
        camera_perspective_update(camera);
        camera_view_update(camera);
    }

    void camera_view_position_update(u32 shader, glm::vec3& position) {
        gl::shader_use(shader);
        shader_set_uniform3v(shader, "view_position", position);
    }

}