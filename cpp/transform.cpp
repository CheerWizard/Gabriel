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
        shader.set_uniform_args("model", model);
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

    void Transform2d::update(Shader &shader) const {
        UniformV2F uniform_translation = { "translation", translation };
        UniformF uniform_rotation = { "rotation", rotation };
        UniformV2F uniform_scale = { "scale", scale };
        shader.set_uniform_struct("transform", uniform_translation);
        shader.set_uniform_struct("transform", uniform_rotation);
        shader.set_uniform_struct("transform", uniform_scale);
    }

    void Transform2d::update_array_element(Shader &shader, int i, const Transform2d &transform) {
        UniformV2F uniform_translation = { "translation", transform.translation };
        UniformF uniform_rotation = { "rotation", transform.rotation };
        UniformV2F uniform_scale = { "scale", transform.scale };
        shader.set_uniform_struct("transform", uniform_translation, i);
        shader.set_uniform_struct("transform", uniform_rotation, i);
        shader.set_uniform_struct("transform", uniform_scale, i);
    }

    void Transform2d::update_array(Shader &shader, const std::vector<Transform2d> &transforms) {
        size_t size = transforms.size();
        for (int i = 0; i < size; i++) {
            auto& transform = transforms[i];
            UniformV2F uniform_translation = { "translation", transform.translation };
            UniformF uniform_rotation = { "rotation", transform.rotation };
            UniformV2F uniform_scale = { "scale", transform.scale };
            shader.set_uniform_struct("transform", uniform_translation, i);
            shader.set_uniform_struct("transform", uniform_rotation, i);
            shader.set_uniform_struct("transform", uniform_scale, i);
        }
    }
}