#include <geometry.h>

namespace gl {

    void free(u32 shader_program, u32 vao, u32 vbo) {
        shader_free(shader_program);
        vao_free(vao);
        vbo_free(vbo);
    }

    void free(u32 shader_program, u32 vao, u32 vbo, u32 ibo) {
        shader_free(shader_program);
        vao_free(vao);
        vbo_free(vbo);
        ibo_free(ibo);
    }

}