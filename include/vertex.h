#pragma once

#include <primitives.h>
#include <shader.h>

#include <glm.hpp>

#include <vector>

namespace gl {

    enum PrimitiveType : int {
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

    struct Attr {
        u32 location;
        PrimitiveType primitive_type;
    };

    namespace attr {
        static constexpr Attr pos = { 0, vec3 };
        static constexpr Attr uv = { 1, vec2 };
        static constexpr Attr normal = { 2, vec3 };
        static constexpr Attr tangent = { 3, vec3 };
        static constexpr Attr bitangent = { 4, vec3 };
        static constexpr Attr bone_id = { 5, vec4 };
        static constexpr Attr weight = { 6, vec4 };
    }

    struct VertexFormat final {
        std::vector<Attr> attrs;
        size_t stride;
    };

#define decl_vertex static VertexFormat format;

    struct VertexDefault final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
    };

    struct VertexUV final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
    };

    struct VertexTBN final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
    };

    struct VertexData final {
        size_t size;
        float* data;
    };

    struct IndexData final {
        size_t size;
        u32* data;
    };

}
