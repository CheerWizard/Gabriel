#include <api/draw.h>

namespace gl {

    void DrawableVertices::free() {
        vao.free();
        vbo.free();
    }

    void DrawableVertices::draw() {
        vao.bind();
        glDrawArrays(type, 0, vertexCount);
    }

    void DrawableVertices::draw(int instances) {
        vao.bind();
        glDrawArraysInstanced(type, 0, vertexCount, instances);
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
            glDrawElements(type, verticesPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(u32) * verticesPerStrip * strip));
        }
    }

    void DrawableElements::draw(int instances) const {
        vao.bind();
        ibo.bind();
        // draws with either triangles or triangle strips
        for (u32 strip = 0; strip < strips; strip++)
        {
            glDrawElementsInstanced(type, verticesPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(u32) * verticesPerStrip * strip), instances);
        }
    }

    void DrawableQuad::init() {
        vao.init();
    }

    void DrawableQuad::free() {
        vao.free();
    }

    void DrawableQuad::draw() const {
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}