#include <transform.h>

#include <glad/glad.h>

namespace gl {

    glm::mat4 Transform::init() const {
        glm::mat4 result(1.0f);
        result = glm::translate(result, translation);
        result = glm::rotate(result, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        result = glm::rotate(result, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        result = glm::rotate(result, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        result = glm::scale(result, scale);
        return result;
    }

    void Transform::update(Shader &shader) const {
        glm::mat4 model = init();
        shader.set_uniform_args<glm::mat4>("model", model);
    }

    void Transform::update_array_element(Shader &shader, int i, const Transform& transform) {
        glm::mat4 model = transform.init();
        shader.set_uniform("models", model, i);
    }

    void Transform::update_array(Shader &shader, const std::vector<Transform> &transforms) {
        size_t size = transforms.size();
        for (int i = 0; i < size; i++) {
            glm::mat4 model = transforms[i].init();
            shader.set_uniform("models", model, i);
        }
    }
}