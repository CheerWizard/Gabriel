#pragma once

#include <draw.h>

#include <glm/glm.hpp>

namespace gl {

    template<typename T>
    struct Vertices {
        T* vertices = null;
        int count = 0;

        inline size_t size() const { return sizeof(T) * count; }
        inline float* to_float() const { return (float*) vertices; }
        inline T& operator [](int i) { return vertices[i]; }

        void init(int count);
        void free();

        void copy_from(Vertices<T>* src);
    };

    template<typename T>
    void Vertices<T>::init(int count) {
        this->count = count;
        vertices = (T*) std::malloc(size());
    }

    template<typename T>
    void Vertices<T>::free() {
        std::free(vertices);
        count = 0;
    }

    template<typename T>
    void Vertices<T>::copy_from(Vertices<T>* src) {
        free();
        init(src->count);
        std::memcpy(vertices, src->vertices, src->size());
    }

    template<typename T>
    static void init_uv(T* v0, T* v1, T* v2, T* v3) {
        v0->uv = { 0, 0 };
        v1->uv = { 0, 1 };
        v2->uv = { 1, 1 };
        v3->uv = { 1, 0 };
    }

    template<typename T>
    static void init_normal(T* v0, T* v1, T* v2, T* v3) {
        glm::vec3 x1 = v1->pos - v0->pos;
        glm::vec3 x2 = v3->pos - v0->pos;
        glm::vec3 normal = -glm::normalize(glm::cross(x1, x2));
        v0->normal = normal;
        v1->normal = normal;
        v2->normal = normal;
        v3->normal = normal;
    }

    template<typename T>
    static void init_tbn(T* v0, T* v1, T* v2, T* v3) {
        glm::vec3& pos0 = v0->pos;
        glm::vec3& pos1 = v1->pos;
        glm::vec3& pos2 = v2->pos;

        glm::vec2& uv0 = v0->uv;
        glm::vec2& uv1 = v1->uv;
        glm::vec2& uv2 = v2->uv;

        // Calculate Triangle edges
        glm::vec3 dpos_1 = pos1 - pos0;
        glm::vec3 dpos_2 = pos2 - pos0;

        // Calculate delta UV
        glm::vec2 duv_1 = uv1 - uv0;
        glm::vec2 duv_2 = uv2 - uv0;

        // Calculate tangent
        float r = 1.0f / (duv_1.x * duv_2.y - duv_1.y * duv_2.x);
        glm::vec3 tangent = (dpos_1 * duv_2.y - dpos_2 * duv_1.y) * r;
        v0->tangent = tangent;
        v1->tangent = tangent;
        v2->tangent = tangent;
        v3->tangent = tangent;
    }

    struct Indices final {
        u32* indices = null;
        int count = 0;

        inline size_t size() const { return sizeof(u32) * count; }
        inline u32& operator [](int i) const { return indices[i]; }

        void init(int count);
        void free();

        void copy_from(Indices* src);
    };

    template<typename T>
    struct Geometry {
        Vertices<T> vertices;
        Indices indices;

        void init_drawable(DrawableElements& drawable);
        void free();
    };

    template<typename T>
    void Geometry<T>::init_drawable(DrawableElements &drawable) {
        drawable.vao.init();
        drawable.vao.bind();
        drawable.vbo.init(vertices, T::format, GL_DYNAMIC_DRAW);
        drawable.ibo.init(indices.indices, drawable.strips * drawable.vertices_per_strip, GL_DYNAMIC_DRAW);
    }

    template<typename T>
    void Geometry<T>::free() {
        vertices.free();
        indices.free();
    }

}
