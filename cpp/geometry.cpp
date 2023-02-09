#include <geometry.h>

namespace gl {

    void triangle_init(u32& shader_program, u32& vao, u32& vbo, const triangle& geometry) {
        shader_program = shader_program_init({ "shaders/triangle.vert", "shaders/triangle.frag" });

        vao = vao_init();
        vao_bind(vao);

        attr_type attrs[] = { vec3 };
        vertex_format format {};
        format.attrs = attrs;
        format.attr_count = sizeof(attrs) / sizeof(attrs[0]);
        format.stride = sizeof(vertex);
        vbo = static_vbo_init(geometry, format);
    }

    void triangle_free(u32 shader_program, u32 vao, u32 vbo) {
        vbo_free(vbo);
        vao_free(vao);
        shader_program_free(shader_program);
    }

    void triangle_init_batch(u32& shader_program, u32& vao, u32& vbo, u32 batch_count, const triangle& base) {
        shader_program = shader_program_init({ "shaders/triangle.vert", "shaders/triangle.frag" });

        vao = vao_init();
        vao_bind(vao);

        for (u32 i = 0 ; i < batch_count ; i++) {

        }
    }

    void rect_init(u32& shader_program, u32& vao, u32& vbo, u32& ibo, const rect& geometry) {
        shader_program = shader_program_init({ "shaders/rect.vert", "shaders/rect.frag" });

        vao = vao_init();
        vao_bind(vao);

        attr_type attrs[] = { vec3 };
        vertex_format format {};
        format.attrs = attrs;
        format.attr_count = sizeof(attrs) / sizeof(attrs[0]);
        format.stride = sizeof(vertex);
        vbo = static_vbo_init(geometry.vertices, format);

        ibo = static_ibo_init(geometry.indices, 6);
    }

    void rect_free(u32 shader_program, u32 vao, u32 vbo, u32 ibo) {
        ibo_free(ibo);
        vbo_free(vbo);
        vao_free(vao);
        shader_program_free(shader_program);
    }

}