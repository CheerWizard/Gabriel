#pragma once

#include <primitives.h>
#include <shader.h>

#include <glm.hpp>

#include <vector>

namespace gl {

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
        std::vector<attr_type> attrs;
        size_t stride;
    };

#define decl_vertex \
static vertex_format format; \
static shader_props shader_props;

    struct vertex_solid final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec4 color = { 0.5 , 0.5, 0.5, 0.5 };
    };

    struct vertex_solid_normal final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec4 color = { 0.5 , 0.5, 0.5, 0.5 };
        glm::fvec3 normal = { 0, 0, 0 };
    };

    struct vertex_uv final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
    };

    struct vertex_uv_normal final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
    };

    struct vertex_data_t final {
        size_t size;
        float* data;
    };

    struct index_data_t final {
        size_t size;
        u32* data;
    };

}
