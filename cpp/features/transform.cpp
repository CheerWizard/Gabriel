#include <features/transform.h>

namespace gl {

    void Transform::update(Shader &shader) {
        UniformM4F model = { "model", value };
        shader.setUniform(model);
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
        UniformM3F model = { "model", value };
        shader.setUniform(model);
    }

    void Transform2d::updateArrayElement(Shader &shader, int i, const Transform2d &transform) {
//        shader.setUniform("models", transform.value, i);
    }

    void Transform2d::updateArray(Shader &shader, const std::vector<Transform2d> &transforms) {
//        size_t size = transforms.size();
//        for (int i = 0; i < size; i++) {
//            shader.setUniform("models", transforms[i].value, i);
//        }
    }
}