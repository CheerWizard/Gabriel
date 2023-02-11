#include <transform.h>

#include <glad/glad.h>

namespace gl {

    glm::mat4 transform_mat(const transform& data) {
        glm::mat4 result(1.0f);
        result = glm::translate(result, data.translation);
        result = glm::rotate(result, glm::radians(data.rotation.x), glm::vec3(1, 0, 0));
        result = glm::rotate(result, glm::radians(data.rotation.y), glm::vec3(0, 1, 0));
        result = glm::rotate(result, glm::radians(data.rotation.z), glm::vec3(0, 0, 1));
        result = glm::scale(result, data.scale);
        return result;
    }

    void transform_update(u32 shader, const transform& transform) {
        glm::mat4 model = transform_mat(transform);
        shader_set_uniform4m(shader, "model", model);
        glm::mat4 normal_model = glm::transpose(glm::inverse(model));
        shader_set_uniform4m(shader, "normal_model", normal_model);
    }

}