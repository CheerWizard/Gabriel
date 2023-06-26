#pragma once

#include <glad/glad.h>

namespace gl {

    template<typename T>
    struct StorageBuffer {
        u32 id;
        long long capacity;

        StorageBuffer() = default;
        StorageBuffer(long long capacity) : capacity(capacity) {}

        void init(u32 binding);
        void free();
        void bind() const;
        void resize(long long capacity);

        void update(long long offset, long long size, void* data);
        void tryUpdate(long long offset, long long size, void* data);

        static void* map();
        static void unmap();
    };

    template<typename T>
    void StorageBuffer<T>::init(u32 binding) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * capacity, null, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
    }

    template<typename T>
    void StorageBuffer<T>::free() {
        glDeleteBuffers(1, &id);
    }

    template<typename T>
    void StorageBuffer<T>::bind() const {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    }

    template<typename T>
    void StorageBuffer<T>::resize(long long capacity) {
        this->capacity = capacity;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * capacity, null, GL_DYNAMIC_COPY);
    }

    template<typename T>
    void StorageBuffer<T>::update(long long offset, long long size, void* data) {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    }

    template<typename T>
    void StorageBuffer<T>::tryUpdate(long long offset, long long size, void* data) {
        bind();

        if (size > capacity) {
            capacity = size;
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
            return;
        }

        update(offset, size, data);
    }

    template<typename T>
    void* StorageBuffer<T>::map() {
        return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    }

    template<typename T>
    void StorageBuffer<T>::unmap() {
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

}