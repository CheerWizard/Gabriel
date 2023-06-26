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

    component(Transform2d), Model2dMat {

        Transform2d() = default;

        Transform2d(const glm::vec2& translation, const float rotation, const glm::vec2& scale) {
            this->translation = translation;
            this->rotation = rotation;
            this->scale = scale;
            init();
        }

        Transform2d(const Transform2d& transform) : Model2dMat(transform) {}

        void update(Shader& shader) const;

        static void updateArrayElement(Shader& shader, int i, const Transform2d& transform);
        static void updateArray(Shader& shader, const std::vector<Transform2d>& transforms);
    };

}