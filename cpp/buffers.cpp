#include <buffers.h>

namespace gl {

    void vbo_set_format(const vertex_format& format) {
        u32 attr_count = format.attr_count;
        int attr_offset = 0;
        size_t stride = 0;
        for (u32 i = 0; i < attr_count; i++) {
            attr_type attr = format.attrs[i];
            attr_offset += attr;
            glVertexAttribPointer(
                    i, attr_offset,
                    GL_FLOAT, GL_FALSE,
                    sizeof(float) * attr, (void *) stride);
            glEnableVertexAttribArray(i);
            stride += format.stride;
        }
    }

    void vbo_bind(u32 vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    void vbo_free(u32 vbo) {
        glDeleteBuffers(1, &vbo);
    }

    u32 static_ibo_init(const u32 indices[], u32 index_count) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, GL_STATIC_DRAW);

        return id;
    }

    void ibo_bind(u32 ibo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }

    void ibo_free(u32 ibo) {
        glDeleteBuffers(1, &ibo);
    }

    u32 vao_init() {
        u32 id;
        glGenVertexArrays(1, &id);
        return id;
    }

    void vao_bind(u32 vao) {
        glBindVertexArray(vao);
    }

    void vao_free(u32 vao) {
        glDeleteVertexArrays(1, &vao);
    }

}