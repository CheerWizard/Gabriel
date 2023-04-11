#include <draw.h>

namespace gl {

    void DrawableVertices::free() {
        vao.free();
        vbo.free();
    }

    void DrawableVertices::draw() {
        vao.bind();
        glDrawArrays(type, 0, vertex_count);
    }

    void DrawableVertices::draw(int instances) {
        vao.bind();
        glDrawArraysInstanced(type, 0, vertex_count, instances);
    }

    void DrawableElements::free() {
        vao.free();
        vbo.free();
        ibo.free();
    }

    void DrawableElements::draw() const {
        vao.bind();
        ibo.bind();
        // draws with either triangles or triangle strips
        for (u32 strip = 0; strip < strips; strip++)
        {
            glDrawElements(type, vertices_per_strip, GL_UNSIGNED_INT, (void*)(sizeof(u32) * vertices_per_strip * strip));
        }
    }

    void DrawableElements::draw(int instances) const {
        vao.bind();
        ibo.bind();
        // draws with either triangles or triangle strips
        for (u32 strip = 0; strip < strips; strip++)
        {
            glDrawElementsInstanced(type, vertices_per_strip, GL_UNSIGNED_INT, (void*)(sizeof(u32) * vertices_per_strip * strip), instances);
        }
    }

}