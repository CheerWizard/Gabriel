#pragma once

#include <api/shader.h>

namespace gl {

    component(Transform), ModelMat {

        Transform() = default;

        Transform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale) {
            this->translation = translation;
            this->rotation = rotation;
            this->scale = scale;
            init();
        }

        Transform(const Transform& transform) : ModelMat(transform) {}

        void update(Shader& shader);

        static void updateArrayElement(Shader& shader, int i, Transform& transform);
        static void updateArray(Shader& shader, std::vector<Transform>& transforms);
    };

    component(Transform2d) {
        glm::vec2 translation = { 0, 0 };
        float rotation = 0;
        glm::vec2 scale = { 1, 1 };

        Transform2d() = default;

        Transform2d(const glm::vec2& translation, const float rotation, const glm::vec2& scale)
        : translation(translation), rotation(rotation), scale(scale) {}

        Transform2d(const Transform2d& transform) = default;

        void update(Shader& shader) const;

        static void updateArrayElement(Shader& shader, int i, const Transform2d& transform);
        static void updateArray(Shader& shader, const std::vector<Transform2d>& transforms);
    };

}