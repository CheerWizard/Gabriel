#include <camera.h>
#include <math_functions.h>
#include <window.h>

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

    camera camera_init(u32 shader) {
        camera new_camera;
        new_camera.aspect = win::get_aspect_ratio();

        gl::shader_use(shader);
        camera_perspective_update(shader, new_camera);
        camera_view_update(shader, new_camera);

        return new_camera;
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

//        clamp(s_camera.pitch, -s_camera.max_pitch, s_camera.max_pitch);

        glm::vec3 direction;
        direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        direction.y = sin(glm::radians(camera.pitch));
        direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(direction);
    }

    void camera_zoom(u32 shader, camera& camera, double x, double y) {
        camera.fov -= (float) y;
        clamp(camera.fov, 1.0f, camera.max_fov);
        gl::shader_use(shader);
        camera_perspective_update(shader, camera);
    }

    void camera_resize(u32 shader, camera& camera, int w, int h) {
        camera.aspect = (float)w / (float)h;
        gl::shader_use(shader);
        camera_perspective_update(shader, camera);
    }

    void camera_view_update(u32 shader, camera& camera) {
        glm::mat4 view = view_mat(camera);
        shader_set_uniform4m(shader, "view", view);
    }

    void camera_perspective_update(u32 shader, camera& camera) {
        glm::mat4 perspective = gl::perspective_mat({
            camera.fov,
            camera.aspect,
            camera.z_near,
            camera.z_far
        });
        gl::shader_set_uniform4m(shader, "perspective", perspective);
    }

}