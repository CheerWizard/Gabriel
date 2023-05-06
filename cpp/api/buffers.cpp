#include <api/buffers.h>

namespace gl {

    void VertexArray::init() {
        glGenVertexArrays(1, &id);
    }

    void VertexArray::free() {
        glDeleteVertexArrays(1, &id);
    }

    void VertexArray::bind() const {
        glBindVertexArray(id);
    }

    void VertexBuffer::init() {
        glGenBuffers(1, &id);
    }

    void VertexBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void VertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void VertexBuffer::setFormat(const VertexFormat &format) {
        size_t offset = 0;
        size_t stride = format.stride;
        for (auto& attribute : format.attrs) {
            glVertexAttribPointer(
                    attribute.location, attribute.primitiveType,
                    GL_FLOAT, GL_FALSE,
                    stride, (void*) offset
            );
            glEnableVertexAttribArray(attribute.location);
            offset += attribute.primitiveType * sizeof(float);
        }
    }

    void IndexBuffer::init(u32 indexCount, BufferAllocType allocType) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indexCount, null, allocType);
    }

    void IndexBuffer::init(u32* indices, u32 indexCount, BufferAllocType allocType) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indexCount, indices, allocType);
    }

    void IndexBuffer::init(const u32* indices, u32 indexCount, BufferAllocType allocType) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indexCount, indices, allocType);
    }

    void IndexBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void IndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void IndexBuffer::update(u32* indices, u32 indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * indexCount, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::update(const u32* indices, u32 indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * indexCount, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}