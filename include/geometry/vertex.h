#pragma once

#include <core/common.h>

#include <api/shader.h>

#include <glm/glm.hpp>

#include <vector>

namespace gl {

    enum PrimitiveType : int {
        Float = 1,
        Boolean = 1,
        Integer = 1,

        Vec2 = 2,
        Vec3 = 3,
        Vec4 = 4,

        Mat2 = 4,
        Mat3 = 9,
        Mat4 = 16,
    };

    struct Attribute {
        u32 location;
        PrimitiveType primitiveType;
    };

    namespace Attributes {
        static constexpr Attribute POS2 = {0, Vec2 };
        static constexpr Attribute POS3 = {0, Vec3 };
        static constexpr Attribute UV = {1, Vec2 };
        static constexpr Attribute NORMAL = {2, Vec3 };
        static constexpr Attribute TANGENT = {3, Vec3 };
        static constexpr Attribute BITANGENT = {4, Vec3 };
        static constexpr Attribute BONE_ID = {5, Vec4 };
        static constexpr Attribute WEIGHT = {6, Vec4 };
    }

    struct VertexFormat final {
        std::vector<Attribute> attrs;
        size_t stride;
    };

#define VERTEX() static VertexFormat format;

    struct VertexDefault final {
        VERTEX()
        glm::fvec3 pos = { 0, 0, 0 };
    };

    struct Vertex2dUV final {
        VERTEX()
        glm::fvec2 pos = { 0, 0 };
        glm::fvec2 uv = { 0, 0 };
    };

    struct VertexUV final {
        VERTEX()
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
    };

    struct VertexTBN final {
        VERTEX()
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
