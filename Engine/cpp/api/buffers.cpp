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

    void VertexBuffer::init(const VertexFormat &format, size_t capacity, void* data, const BufferAllocType allocType) {
        this->capacity = capacity;
        init();
        bind();
        glBufferData(GL_ARRAY_BUFFER, capacity, data, allocType);
        setFormat(format);
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

    void VertexBuffer::update(size_t offset, size_t size, void* data) const {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::tryUpdate(size_t offset, size_t size, void* data) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        if (size > capacity) {
            capacity = size;
            glBufferData(GL_ARRAY_BUFFER, capacity, data, BufferAllocType::DYNAMIC);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::init(u32 indexCount, BufferAllocType allocType) {
        glGenBuffers(1, &id);
        capacity = sizeof(u32) * indexCount;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, capacity, null, allocType);
    }

    void IndexBuffer::init(u32* indices, u32 indexCount, BufferAllocType allocType) {
        glGenBuffers(1, &id);
        capacity = sizeof(u32) * indexCount;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, capacity, indices, allocType);
    }

    void IndexBuffer::init(const u32* indices, u32 indexCount, BufferAllocType allocType) {
        glGenBuffers(1, &id);
        capacity = sizeof(u32) * indexCount;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, capacity, indices, allocType);
    }

    void IndexBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void IndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void IndexBuffer::update(u32* indices, u32 indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(u32) * indexCount, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::update(const u32* indices, u32 indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(u32) * indexCount, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::update(const std::vector<u32>& indices) {
        update(indices.data(), indices.size());
    }

    void IndexBuffer::tryUpdate(u32* indices, u32 indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

        size_t size = sizeof(u32) * indexCount;
        if (size > capacity) {
            capacity = size;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, BufferAllocType::DYNAMIC);
        } else {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::tryUpdate(const u32* indices, u32 indexCount) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

        size_t size = sizeof(u32) * indexCount;
        if (size > capacity) {
            capacity = size;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, BufferAllocType::DYNAMIC);
        } else {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::tryUpdate(const std::vector<u32>& indices) {
        tryUpdate(indices.data(), indices.size());
    }

}