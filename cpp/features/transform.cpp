#include <features/transform.h>

namespace gl {

    void Transform::update(Shader &shader) {
        shader.setUniformArgs("model", value);
    }

    void Transform::updateArrayElement(Shader &shader, int i, Transform& transform) {
        shader.setUniform("models", transform.value, i);
    }

    void Transform::updateArray(Shader &shader, std::vector<Transform> &transforms) {
        size_t size = transforms.size();
        for (int i = 0; i < size; i++) {
            shader.setUniform("models", transforms[i].value, i);
        }
    }

    void Transform2d::update(Shader &shader) const {
        UniformV2F uniform_translation = { "translation", translation };
        UniformF uniform_rotation = { "rotation", rotation };
        UniformV2F uniform_scale = { "scale", scale };
        shader.setUniformStruct("transform", uniform_translation);
        shader.setUniformStruct("transform", uniform_rotation);
        shader.setUniformStruct("transform", uniform_scale);
    }

    void Transform2d::updateArrayElement(Shader &shader, int i, const Transform2d &transform) {
        UniformV2F uniform_translation = { "translation", transform.translation };
        UniformF uniform_rotation = { "rotation", transform.rotation };
        UniformV2F uniform_scale = { "scale", transform.scale };
        shader.setUniformStruct("transform", uniform_translation, i);
        shader.setUniformStruct("transform", uniform_rotation, i);
        shader.setUniformStruct("transform", uniform_scale, i);
    }

    void Transform2d::updateArray(Shader &shader, const std::vector<Transform2d> &transforms) {
        size_t size = transforms.size();
        for (int i = 0; i < size; i++) {
            auto& transform = transforms[i];
            UniformV2F uniform_translation = { "translation", transform.translation };
            UniformF uniform_rotation = { "rotation", transform.rotation };
            UniformV2F uniform_scale = { "scale", transform.scale };
            shader.setUniformStruct("transform", uniform_translation, i);
            shader.setUniformStruct("transform", uniform_rotation, i);
            shader.setUniformStruct("transform", uniform_scale, i);
        }
    }
}