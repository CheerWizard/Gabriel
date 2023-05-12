#pragma once

#include <api/shader.h>
#include <api/buffers.h>
#include <api/draw.h>

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
        inline float* toFloat() const { return (float*) &v0.pos.x; }

        void init(DrawableVertices& drawable);
    };

    typedef Triangle<VertexUV> TriangleUV;

    template<typename T>
    void Triangle<T>::init(DrawableVertices &drawable)
    {
        drawable.vao.bind();
        drawable.vbo.init(this, T::format, BufferAllocType::STATIC);
    }

}