#include <api/storage.h>

#include <glad/glad.h>

namespace gl {

    void StorageBuffer::init(u32 binding, long long size) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, null, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
    }

    void StorageBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void StorageBuffer::bind() const {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    }

    void StorageBuffer::update(long long offset, long long size, void* data) const {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
//        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
//        memcpy(p, data, size);
//        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

}