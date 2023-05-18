#pragma once

#include <geometry/vertex.h>

#include <glad/glad.h>

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
        size_t capacity;

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

        template<typename T>
        void tryUpdate(const T& geometry, size_t offset = 0);
        template<typename T>
        void tryUpdate(const std::vector<T>& geometries);

        void update(size_t offset, size_t size, void* data) const;
        void tryUpdate(size_t offset, size_t size, void* data);

        void free();
    };

    template<typename T>
    void VertexBuffer::init(const T& geometry, const VertexFormat& format, BufferAllocType allocType) {
        glGenBuffers(1, &id);
        capacity = geometry.size();
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, capacity, geometry.toFloat(), allocType);
        setFormat(format);
    }

    template<typename T>
    void VertexBuffer::init(const std::vector<T>& geometries, const VertexFormat& format, BufferAllocType allocType) {
        glGenBuffers(1, &id);
        capacity = sizeof(T) * geometries.size();
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, capacity, geometries.begin()->toFloat(), allocType);
        setFormat(format);
    }

    template<typename T>
    void VertexBuffer::init(size_t size, const VertexFormat& format, BufferAllocType allocType) {
        glGenBuffers(1, &id);
        capacity = sizeof(T) * size;
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, capacity, null, allocType);
        setFormat(format);
    }

    template<typename T>
    void VertexBuffer::update(const T& geometry, size_t offset) {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, geometry.size(), geometry.toFloat());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void VertexBuffer::update(const std::vector<T>& geometries) {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        size_t size = geometries.size() * sizeof(T);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, geometries.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void VertexBuffer::tryUpdate(const T& geometry, size_t offset) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        size_t size = geometry.size();
        if (size > capacity) {
            capacity = size;
            glBufferData(GL_ARRAY_BUFFER, capacity, geometry.toFloat(), BufferAllocType::DYNAMIC);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, geometry.toFloat());
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    template<typename T>
    void VertexBuffer::tryUpdate(const std::vector<T>& geometries) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        size_t size = geometries.size() * sizeof(T);
        if (size > capacity) {
            capacity = size;
            glBufferData(GL_ARRAY_BUFFER, capacity, geometries.data(), BufferAllocType::DYNAMIC);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, geometries.data());
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    struct IndexBuffer final {
        u32 id;
        size_t capacity;

        void init(u32 indexCount, BufferAllocType allocType);
        void init(u32* indices, u32 indexCount, BufferAllocType allocType);
        void init(const u32* indices, u32 indexCount, BufferAllocType allocType);

        void free();

        void bind() const;

        void update(u32* indices, u32 indexCount);
        void update(const u32* indices, u32 indexCount);
        void update(const std::vector<u32>& indices);
        void tryUpdate(u32* indices, u32 indexCount);
        void tryUpdate(const u32* indices, u32 indexCount);
        void tryUpdate(const std::vector<u32>& indices);
    };
}
