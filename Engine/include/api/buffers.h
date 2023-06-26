#pragma once

#include <geometry/vertex.h>

#include <glad/glad.h>

namespace gl {

    struct GABRIEL_API VertexArray final {
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

    struct GABRIEL_API VertexBuffer final {
        u32 id;
        size_t capacity;

        void init();

        void init(const VertexFormat& format, size_t capacity, void* data, const BufferAllocType allocType);

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
        void tryUpdate(std::vector<T>& geometries);

        void update(size_t offset, size_t size, void* data) const;
        void tryUpdate(size_t offset, size_t size, void* data);

        void free();
    };

    template<typename T>
    void VertexBuffer::init(const T& geometry, const VertexFormat& format, BufferAllocType allocType) {
        init(format, geometry.size(), geometry.toFloat(), allocType);
    }

    template<typename T>
    void VertexBuffer::init(const std::vector<T>& geometries, const VertexFormat& format, BufferAllocType allocType) {
        init(format, sizeof(T) * geometries.size(), geometries.begin()->toFloat(), allocType);
    }

    template<typename T>
    void VertexBuffer::init(size_t size, const VertexFormat& format, BufferAllocType allocType) {
        init(format, sizeof(T) * size, null, allocType);
    }

    template<typename T>
    void VertexBuffer::update(const T& geometry, size_t offset) {
        update(offset, geometry.size(), geometry.toFloat());
    }

    template<typename T>
    void VertexBuffer::update(const std::vector<T>& geometries) {
        update(0, geometries.size() * sizeof(T), geometries.data());
    }

    template<typename T>
    void VertexBuffer::tryUpdate(const T& geometry, size_t offset) {
        tryUpdate(offset, geometry.size(), geometry.toFloat());
    }

    template<typename T>
    void VertexBuffer::tryUpdate(std::vector<T>& geometries) {
        tryUpdate(0, geometries.size() * sizeof(T), geometries.data());
    }

    struct GABRIEL_API IndexBuffer final {
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
