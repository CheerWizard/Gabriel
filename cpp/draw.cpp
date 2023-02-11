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

}