#pragma once

#include <primitives.h>
#include <shader.h>

#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace gl {

    struct transform final {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::fvec3 scale = { 1, 1, 1 };
    };

    glm::mat4 transform_mat(const transform& data);

    void transform_update(u32 shader, const transform& transform);

}