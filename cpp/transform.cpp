#include <transform.h>

namespace gl {

    glm::mat4 mat_model(const transform& data) {
        glm::mat4 result(1.0f);
        result = glm::translate(result, data.translation);
        result = glm::rotate(result, glm::radians(data.rotation.x), glm::vec3(1, 0, 0));
        result = glm::rotate(result, glm::radians(data.rotation.y), glm::vec3(0, 1, 0));
        result = glm::rotate(result, glm::radians(data.rotation.z), glm::vec3(0, 0, 1));
        result = glm::scale(result, data.scale);
        return result;
    }

    void shader_set_model(u32 shader, const transform& data) {
        shader_set_uniform4m(shader, "model", mat_model(data));
    }

    glm::mat4 mat_view(const camera& data) {
        return glm::lookAt(
                data.position,
                data.position + data.front,
                data.up
        );
    }

    void shader_set_camera(u32 shader, const camera& camera) {
        shader_set_uniform4m(shader, "view", mat_view(camera));
    }

    glm::mat4 mat_ortho(const ortho_data& data) {
        return glm::ortho(data.left, data.right, data.bottom, data.top, data.zNear, data.zFar);
    }

    glm::mat4 mat_perspective(const perspective_data& data) {
        return glm::perspective(glm::radians(data.fov_degree), data.aspect_ratio, data.zNear, data.zFar);
    }

}