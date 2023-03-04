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
        glDrawElements(type, index_count, GL_UNSIGNED_INT, null);
    }

    void DrawableElements::draw(int instances) const {
        vao.bind();
        ibo.bind();
        glDrawElementsInstanced(type, index_count, GL_UNSIGNED_INT, null, instances);
    }

}