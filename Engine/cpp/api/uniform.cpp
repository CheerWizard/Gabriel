#include <api/uniform.h>

#include <glad/glad.h>

namespace gl {

    void UniformBuffer::init(const u32 binding, long long size) {
        glGenBuffers(1, &m_id);

        glBindBuffer(GL_UNIFORM_BUFFER, m_id);
        glBufferData(GL_UNIFORM_BUFFER, size, null, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
    }

    void UniformBuffer::free() {
        glDeleteBuffers(1, &m_id);
    }

    void UniformBuffer::bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    }

    void UniformBuffer::update(long long offset, long long size, const void* data) const {
        glBindBuffer(GL_UNIFORM_BUFFER, m_id);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }

}