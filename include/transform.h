#pragma once

#include <primitives.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace gl {

    struct Transform final {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::fvec3 scale = { 1, 1, 1 };

        glm::mat4 init() const;

        void update(Shader& shader) const;

        static void update_array_element(Shader& shader, int i, const Transform& transform);

        static void update_array(Shader& shader, const std::vector<Transform>& transforms);
    };

}