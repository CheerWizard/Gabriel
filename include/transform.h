#pragma once

#include <component.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace gl {

    component(Transform) {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::fvec3 scale = { 1, 1, 1 };

        Transform() = default;

        Transform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
        : translation(translation), rotation(rotation), scale(scale) {}

        Transform(const Transform& transform) = default;

        glm::mat4 init() const;

        void update(Shader& shader) const;

        static void update_array_element(Shader& shader, int i, const Transform& transform);

        static void update_array(Shader& shader, const std::vector<Transform>& transforms);
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

        static void update_array_element(Shader& shader, int i, const Transform2d& transform);

        static void update_array(Shader& shader, const std::vector<Transform2d>& transforms);
    };

}