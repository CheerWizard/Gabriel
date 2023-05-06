#pragma once

#include <geometry/vertex.h>

#include <glad/glad.h>

#include <vector>

namespace gl {

    struct VertexArray final {
        u32 id;

        void init();
        void free();
        void bind() const;
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
        void init(const T& geometry, const VertexFormat& format, BufferAllocType allocType);
        template<typename T>
        void init(const std::vector<T>& geometries, const VertexFormat& format, BufferAllocType allocType);
        template<typename T>
        void init(size_t size, const VertexFormat& format, BufferAllocType allocType);

        void bind() const;

        static void setFormat(const VertexFormat& format);

        template<typename T>
        void update(const T& geometry, size_t offset = 0);
        template<typename T>
        void update(const std::vector<T>& geometries);

        void free();
    };

    template<typename T>
    void VertexBuffer::init(const T& geometry, const VertexFormat& format, BufferAllocType allocType) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertexData {};
        vertexData.size = geometry.size();
        vertexData.data = geometry.toFloat();
        glBufferData(GL_ARRAY_BUFFER, vertexData.size, vertexData.data, allocType);

        setFormat(format);
    }

    template<typename T>
    void VertexBuffer::init(const std::vector<T>& geometries, const VertexFormat& format, BufferAllocType allocType) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertexData {};
        vertexData.size = sizeof(T) * geometries.size();
        vertexData.data = geometries.begin()->toFloat();
        glBufferData(GL_ARRAY_BUFFER, vertexData.size, vertexData.data, allocType);

        setFormat(format);
    }

    template<typename T>
    void VertexBuffer::init(size_t size, const VertexFormat& format, BufferAllocType allocType) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * size, null, allocType);

        setFormat(format);
    }

    template<typename T>
    void VertexBuffer::update(const T& geometry, size_t offset) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        VertexData vertexData {};
        vertexData.size = geometry.size();
        vertexData.data = geometry.toFloat();
        glBufferSubData(GL_ARRAY_BUFFER, offset, vertexData.size, vertexData.data);

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

        size_t geometriesSize = geometries.size();
        size_t geometrySize = sizeof(T);
        for (int i = 0 ; i < geometriesSize ; i++) {
            glBufferSubData(GL_ARRAY_BUFFER, i * geometrySize, geometrySize, geometries[i].toFloat());
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    struct IndexBuffer final {
        u32 id;

        void init(u32 indexCount, BufferAllocType allocType);
        void init(u32* indices, u32 indexCount, BufferAllocType allocType);
        void init(const u32* indices, u32 indexCount, BufferAllocType allocType);

        void free();

        void bind() const;

        void update(u32* indices, u32 indexCount);
        void update(const u32* indices, u32 indexCount);
    };
}
