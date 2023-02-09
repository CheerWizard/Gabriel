#pragma once

#include <primitives.h>

#include <glm.hpp>

namespace gl {

    struct vertex final {
        glm::fvec3 pos = { 0, 0, 0 };
    };

    struct vertex_data_t final {
        size_t size;
        float* data;
    };

    struct index_data_t final {
        size_t size;
        u32* data;
    };

    enum attr_type : int {
        float_t = 1,
        bool_t = 1,
        int_t = 1,

        vec2 = 2,
        vec3 = 3,
        vec4 = 4,

        mat2 = 4,
        mat3 = 9,
        mat4 = 16,
    };

    struct vertex_format final {
        attr_type* attrs;
        u32 attr_count;
        size_t stride;
    };

}
