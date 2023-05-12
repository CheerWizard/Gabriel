#pragma once

#include <api/shader.h>
#include <api/buffers.h>
#include <api/draw.h>

#include <geometry/geometry.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct RectVertices {
        T v0 = { { -0.5f, -0.5f } };
        T v1 = { { -0.5f, 0.5f } };
        T v2 = { { 0.5f, 0.5f } };
        T v3 = { { 0.5, -0.5f } };

        inline size_t size() const { return sizeof(RectVertices<T>); }
        inline float* toFloat() const { return (float*) &v0.pos.x; }
    };

    template<typename T>
    struct Rect {
        RectVertices<T> vertices;
        u32 indices[6] = {
                0, 1, 2,
                0, 2, 3
        };

        void init(DrawableElements& drawable);
        void initUV(DrawableElements& drawable);
        void initNormalUV(DrawableElements& drawable);
        void initTBN(DrawableElements& drawable);
    };

    typedef Rect<VertexUV> RectUV;
    typedef Rect<VertexTBN> RectTBN;

    template<typename T>
    void Rect<T>::init(DrawableElements& drawable)
    {
        drawable.vao.init();
        drawable.vao.bind();
        drawable.vbo.init(vertices, T::format, BufferAllocType::STATIC);
        drawable.ibo.init(indices, 6, BufferAllocType::STATIC);
        drawable.strips = 1;
        drawable.verticesPerStrip = 6;
    }

    template<typename T>
    void Rect<T>::initUV(DrawableElements &drawable) {
        gl::initUV(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        init(drawable);
    }

    template<typename T>
    void Rect<T>::initNormalUV(DrawableElements &drawable) {
        gl::initUV(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        gl::initNormal(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        init(drawable);
    }

    template<typename T>
    void Rect<T>::initTBN(DrawableElements &drawable) {
        gl::initUV(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        gl::initNormal(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        gl::initTBN(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        init(drawable);
    }

    typedef Rect<Vertex2dUV> Rect2dUV;

}