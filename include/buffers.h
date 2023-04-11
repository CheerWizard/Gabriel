#pragma once

#include <vertex.h>

#include <glad/glad.h>

#include <vector>

namespace gl {

    struct VertexArray final {
        u32 id;

        void init();
        void bind() const;
        void free();
        void draw_quad() const;
    };

    struct VertexBuffer final {
        u32 id;

        void init();
        template<typename T>
        void init(const T& geometry, const VertexFormat& format, int alloc_type);
        template<typename T>
        void init(const std::vector<T>& geometries, const VertexFormat& format, int alloc_type);

        void bind() const;

        static void set_format(const VertexFormat& format);
        template<typename T>
        void update(const T& geometry);

        void free();
    };

    template<typename T>
    void VertexBuffer::init(const T &geometry, const VertexFormat &format, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertex_data {};
        vertex_data.size = geometry.size();
        vertex_data.data = geometry.to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::init(const std::vector<T> &geometries, const VertexFormat &format, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertex_data {};
        vertex_data.size = sizeof(T) * geometries.size();
        vertex_data.data = geometries.begin()->to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::update(const T& geometry) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        VertexData vertex_data {};
        vertex_data.size = geometry.size();
        vertex_data.data = geometry.to_float();
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data.size, vertex_data.data);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    struct IndexBuffer final {
        u32 id;

        void init(u32* indices, u32 index_count, int alloc_type);
        void init(const u32* indices, u32 index_count, int alloc_type);

        void free();

        void bind() const;

        void update(u32* indices, u32 index_count);
        void update(const u32* indices, u32 index_count);
    };
}
