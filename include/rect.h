#pragma once

#include <shader.h>
#include <buffers.h>
#include <draw.h>
#include <geometry.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct RectVertices final {
        T v0 = { { 0.5f, 0.5f, 0 } };
        T v1 = { { 0.5f, -0.5f, 0 } };
        T v2 = { { -0.5f, -0.5f, 0 } };
        T v3 = { { -0.5, 0.5f, 0 } };

        inline size_t size() const { return sizeof(RectVertices<T>); }
        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    template<typename T>
    static void init_uv(RectVertices<T>& vertices) {
        vertices.v0.uv = { 0, 0 };
        vertices.v1.uv = { 0, 1 };
        vertices.v2.uv = { 1, 1 };
        vertices.v3.uv = { 1, 0 };
    }

    template<typename T>
    static void init_normal(RectVertices<T>& vertices) {
        glm::vec3 normal = -glm::normalize(glm::cross(
                vertices.v1.pos - vertices.v0.pos,
                vertices.v3.pos - vertices.v0.pos
        ));
        vertices.v0.normal = normal;
        vertices.v1.normal = normal;
        vertices.v2.normal = normal;
        vertices.v3.normal = normal;
    }

    template<typename T>
    static void init_tbn(RectVertices<T>& vertices) {
        init_tbn(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
    }

    template<typename T>
    struct Rect final {
        RectVertices<T> vertices;
        u32 indices[6] = {
                0, 1, 3,
                1, 2, 3
        };

        void init(DrawableElements& drawable);
        void init_uv(DrawableElements& drawable);
        void init_normal_uv(DrawableElements& drawable);
        void init_tbn(DrawableElements& drawable);
    };

    typedef Rect<VertexSolid> RectSolid;
    typedef Rect<VertexUV> RectUV;
    typedef Rect<VertexSolidNormal> RectSolidNormal;
    typedef Rect<VertexUVNormal> RectUVNormal;
    typedef Rect<VertexTBN> RectTBN;

    template<typename T>
    void Rect<T>::init(DrawableElements& drawable)
    {
        drawable.vao.init();
        drawable.vao.bind();
        drawable.vbo.init(vertices, T::format, GL_STATIC_DRAW);
        drawable.ibo.init(indices, 6, GL_STATIC_DRAW);
        drawable.index_count = 6;
    }

    template<typename T>
    void Rect<T>::init_uv(DrawableElements &drawable) {
        gl::init_uv(vertices);
        init(drawable);
    }

    template<typename T>
    void Rect<T>::init_normal_uv(DrawableElements &drawable) {
        gl::init_uv(vertices);
        gl::init_normal(vertices);
        init(drawable);
    }

    template<typename T>
    void Rect<T>::init_tbn(DrawableElements &drawable) {
        gl::init_uv(vertices);
        gl::init_normal(vertices);
        gl::init_tbn(vertices);
        init(drawable);
    }

    template<typename T>
    std::vector<Rect<T>> rects_init(
            DrawableElements& drawable,
            u32 count,
            const std::function<Rect<T>(u32)>& rect_factory_fn = [](u32 i) { return Rect<T>(); })
    {
        std::vector<RectVertices<T>> vertices;
        std::vector<u32> indices;
        std::vector<Rect<T>> rects;

        drawable.vao.init();
        drawable.vao.bind();
        vertices.reserve(count * 4);
        indices.reserve(count * 6);
        int index_offset = 0;
        for (u32 i = 0 ; i < count ; i++) {
            Rect<T> new_rect = rect_factory_fn(i);
            vertices.emplace_back(new_rect.vertices);
            for (u32& index : new_rect.indices) {
                indices.emplace_back(index + index_offset);
            }
            rects.emplace_back(new_rect);
            index_offset += 4;
        }
        drawable.vbo.init(vertices, T::format, GL_STATIC_DRAW);
        drawable.ibo.init(indices.data(), indices.size(), GL_STATIC_DRAW);

        return rects;
    }

}