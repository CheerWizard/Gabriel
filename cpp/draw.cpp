#include <draw.h>

namespace gl {

    void drawable_free(const drawable_vertices& vertices) {
        vao_free(vertices.vao);
        vbo_free(vertices.vbo);
    }

    void drawable_free(const drawable_elements& elements) {
        vao_free(elements.vao);
        vbo_free(elements.vbo);
        ibo_free(elements.ibo);
    }

    void draw(const drawable_vertices& vertices) {
        glBindVertexArray(vertices.vao);
        glDrawArrays(vertices.type, 0, vertices.vertex_count);
    }

    void draw(const drawable_vertices& vertices, int instance_count) {
        glBindVertexArray(vertices.vao);
        glDrawArraysInstanced(vertices.type, 0, vertices.vertex_count, instance_count);
    }

    void draw(const drawable_elements& elements) {
        glBindVertexArray(elements.vao);
        ibo_bind(elements.ibo);
        glDrawElements(elements.type, elements.index_count, GL_UNSIGNED_INT, null);
    }

    void draw(const drawable_elements& elements, int instance_count) {
        glBindVertexArray(elements.vao);
        ibo_bind(elements.ibo);
        glDrawElementsInstanced(elements.type, elements.index_count, GL_UNSIGNED_INT, null, instance_count);
    }

    void draw_quad(u32 vao) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}