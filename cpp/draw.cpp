#include <draw.h>

namespace gl {

    void draw_triangle(u32 vao) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void draw_triangles(u32 vao, int count) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * count);
    }

    void draw_rect(u32 vao, u32 ibo) {
        glBindVertexArray(vao);
        ibo_bind(ibo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void draw_rects(u32 vao, u32 ibo, int count) {
        glBindVertexArray(vao);
        ibo_bind(ibo);
        glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_INT, 0);
    }

    void draw_cube(u32 vao, u32 ibo) {
        glBindVertexArray(vao);
        ibo_bind(ibo);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void draw_cubes(u32 vao, u32 ibo, int count) {
        glBindVertexArray(vao);
        ibo_bind(ibo);
        glDrawElements(GL_TRIANGLES, 36 * count, GL_UNSIGNED_INT, 0);
    }

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
        glDrawArrays(GL_TRIANGLES, 0, vertices.vertex_count);
    }

    void draw(const drawable_vertices& vertices, u32 instance_count) {
        glBindVertexArray(vertices.vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.vertex_count, instance_count);
    }

    void draw(const drawable_elements& elements) {
        glBindVertexArray(elements.vao);
        ibo_bind(elements.ibo);
        glDrawElements(GL_TRIANGLES, elements.index_count, GL_UNSIGNED_INT, 0);
    }

    void draw(const drawable_elements& elements, u32 instance_count) {
        glBindVertexArray(elements.vao);
        ibo_bind(elements.ibo);
        glDrawElementsInstanced(GL_TRIANGLES, elements.index_count, GL_UNSIGNED_INT, 0, instance_count);
    }

    void draw_quad(u32 vao) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}