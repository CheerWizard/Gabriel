#include <geometry.h>

namespace gl {

    static attr_type s_vertex_attrs[] = { vec3 };

    static vertex_format get_vertex_format() {
        vertex_format format {};
        format.attrs = s_vertex_attrs;
        format.attr_count = 1;
        format.stride = sizeof(vertex);
        return format;
    }

    triangle triangle_init(u32& shader_program, u32& vao, u32& vbo) {
        triangle result;

        shader_program = shader_program_init({ "shaders/triangle.vert", "shaders/triangle.frag" });

        vao = vao_init();
        vao_bind(vao);
        static_vbo_init(result, get_vertex_format());

        return result;
    }

    void triangle_free(u32 shader_program, u32 vao, u32 vbo) {
        vbo_free(vbo);
        vao_free(vao);
        shader_program_free(shader_program);
    }

    std::vector<triangle> triangles_init(u32& shader_program, u32& vao, u32& vbo, u32 count,
                                         triangle_factory_fn_t triangle_factory_fn) {
        std::vector<triangle> triangles;

        shader_program = shader_program_init({ "shaders/triangle.vert", "shaders/triangle.frag" });

        vao = vao_init();
        vao_bind(vao);

        triangles.reserve(count);
        for (u32 i = 0 ; i < count ; i++) {
            triangles.emplace_back(triangle_factory_fn(i));
        }
        vbo = static_vbo_init(triangles, get_vertex_format());

        return triangles;
    }

    void rect_init(u32& shader_program, u32& vao, u32& vbo, u32& ibo, const rect& geometry) {
        shader_program = shader_program_init({ "shaders/rect.vert", "shaders/rect.frag" });
        vao = vao_init();
        vao_bind(vao);
        vbo = static_vbo_init(geometry.vertices, get_vertex_format());
        ibo = static_ibo_init(geometry.indices, 6);
    }

    void rect_free(u32 shader_program, u32 vao, u32 vbo, u32 ibo) {
        ibo_free(ibo);
        vbo_free(vbo);
        vao_free(vao);
        shader_program_free(shader_program);
    }

}