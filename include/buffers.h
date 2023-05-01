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
    };

    enum BufferAllocType : int {
        STATIC = GL_STATIC_DRAW,
        DYNAMIC = GL_DYNAMIC_DRAW,
        STREAM = GL_STREAM_DRAW
    };

    struct VertexBuffer final {
        u32 id;

        void init();
        template<typename T>
        void init(const T& geometry, const VertexFormat& format, BufferAllocType alloc_type);
        template<typename T>
        void init(const std::vector<T>& geometries, const VertexFormat& format, BufferAllocType alloc_type);
        template<typename T>
        void init(size_t size, const VertexFormat& format, BufferAllocType alloc_type);

        void bind() const;

        static void set_format(const VertexFormat& format);

        template<typename T>
        void update(const T& geometry, size_t offset = 0);
        template<typename T>
        void update(const std::vector<T>& geometries);

        void free();
    };

    template<typename T>
    void VertexBuffer::init(const T &geometry, const VertexFormat &format, BufferAllocType alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertex_data {};
        vertex_data.size = geometry.size();
        vertex_data.data = geometry.to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::init(const std::vector<T> &geometries, const VertexFormat &format, BufferAllocType alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertex_data {};
        vertex_data.size = sizeof(T) * geometries.size();
        vertex_data.data = geometries.begin()->to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::init(size_t size, const VertexFormat& format, BufferAllocType alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * size, null, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::update(const T& geometry, size_t offset) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        VertexData vertex_data {};
        vertex_data.size = geometry.size();
        vertex_data.data = geometry.to_float();
        glBufferSubData(GL_ARRAY_BUFFER, offset, vertex_data.size, vertex_data.data);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void VertexBuffer::update(const std::vector<T>& geometries) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

//        VertexData vertex_data {};
//        auto geometry = geometries[0];
//        vertex_data.size = geometry.size() * geometries.size();
//        vertex_data.data = geometry.to_float();
//        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data.size, vertex_data.data);

        size_t geometries_size = geometries.size();
        size_t geometry_size = sizeof(T);
        for (int i = 0 ; i < geometries_size ; i++) {
            glBufferSubData(GL_ARRAY_BUFFER, i * geometry_size, geometry_size, geometries[i].to_float());
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    struct IndexBuffer final {
        u32 id;

        void init(u32 index_count, BufferAllocType alloc_type);
        void init(u32* indices, u32 index_count, BufferAllocType alloc_type);
        void init(const u32* indices, u32 index_count, BufferAllocType alloc_type);

        void free();

        void bind() const;

        void update(u32* indices, u32 index_count);
        void update(const u32* indices, u32 index_count);
    };
}
