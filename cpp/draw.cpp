#include <draw.h>

namespace gl {

    void draw_triangle(u32 shader_program, u32 vao) {
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void draw_triangles(u32 shader_program, u32 vao, int count) {
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * count);
    }

    void draw_rect(u32 shader_program, u32 vao, u32 ibo) {
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        ibo_bind(ibo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void draw_rects(u32 shader_program, u32 vao, u32 ibo, int count) {
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        ibo_bind(ibo);
        glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_INT, 0);
    }

}