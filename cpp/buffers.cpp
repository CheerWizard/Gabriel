#include <buffers.h>

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

    void VertexArray::draw_quad() const {
        glBindVertexArray(id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
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

    void VertexBuffer::set_format(const VertexFormat &format) {
        u32 attr_count = format.attrs.size();
        size_t attr_offset = 0;
        size_t stride = format.stride;
        for (auto& attr : format.attrs) {
            glVertexAttribPointer(
                    attr.location, attr.primitive_type,
                    GL_FLOAT, GL_FALSE,
                    stride, (void *) attr_offset
            );
            glEnableVertexAttribArray(attr.location);
            attr_offset += attr.primitive_type * sizeof(float);
        }
    }

    void IndexBuffer::init(u32 *indices, u32 index_count, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, alloc_type);
    }

    void IndexBuffer::init(const u32 *indices, u32 index_count, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, alloc_type);
    }

    void IndexBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void IndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void IndexBuffer::update(u32* indices, u32 index_count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * index_count, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::update(const u32* indices, u32 index_count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * index_count, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}