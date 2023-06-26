#pragma once

#include <api/shader.h>

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

    struct GABRIEL_API Attribute {
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

    struct GABRIEL_API VertexFormat final {
        std::vector<Attribute> attrs;
        size_t stride;
    };

#define VERTEX() static VertexFormat format;

    struct GABRIEL_API VertexDefault final {
        VERTEX()
        glm::fvec3 pos = { 0, 0, 0 };
    };

    struct GABRIEL_API Vertex2dUV final {
        VERTEX()
        glm::fvec2 pos = { 0, 0 };
        glm::fvec2 uv = { 0, 0 };
    };

    struct GABRIEL_API VertexUV final {
        VERTEX()
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
    };

    struct GABRIEL_API VertexTBN final {
        VERTEX()
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
    };

    struct GABRIEL_API VertexData final {
        size_t size;
        float* data;
    };

    struct GABRIEL_API IndexData final {
        size_t size;
        u32* data;
    };

}
