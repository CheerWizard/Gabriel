#pragma once

#include <shader.h>
#include <buffers.h>
#include <draw.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct Triangle {
        T v0 = { { -0.5f, -0.5f, 0 } };
        T v1 = { { 0.5f, -0.5f, 0 } };
        T v2 = { { 0, 0.5f, 0 } };

        inline size_t size() const { return sizeof(Triangle<T>); }
        inline float* to_float() const { return (float*) &v0.pos.x; }

        void init(DrawableVertices& drawable);
    };

    typedef Triangle<VertexSolid> TriangleSolid;
    typedef Triangle<VertexUV> TriangleUV;
    typedef Triangle<VertexSolidNormal> TriangleSolidNormal;
    typedef Triangle<VertexUVNormal> TriangleUVNormal;

    template<typename T>
    void Triangle<T>::init(DrawableVertices &drawable)
    {
        drawable.vao.bind();
        drawable.vbo.init(this, T::format, GL_STATIC_DRAW);
    }

    template<typename T>
    std::vector<Triangle<T>> triangles_init(
            DrawableVertices& drawable,
            u32 count,
            const std::function<Triangle<T>(u32)>& triangle_factory_fn = [](u32 i) { return Triangle<T>(); }
    ) {
        std::vector<Triangle<T>> triangles;

        drawable.vao.bind();
        triangles.reserve(count);
        for (u32 i = 0 ; i < count ; i++) {
            triangles.emplace_back(triangle_factory_fn(i));
        }
        drawable.vbo.init(triangles, T::format, GL_STATIC_DRAW);

        return triangles;
    }

}