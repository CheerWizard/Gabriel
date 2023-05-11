#include <api/uniform.h>

#include <glad/glad.h>

namespace gl {

    void UniformBuffer::init(u32 binding, long long size) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, size, null, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    }

    void UniformBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void UniformBuffer::bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void UniformBuffer::update(long long offset, long long size, void* data) const {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }

}