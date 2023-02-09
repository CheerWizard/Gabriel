#pragma once

#include <shader.h>
#include <buffers.h>

#include <gtc/type_ptr.hpp>

#include <vector>

namespace gl {

    struct triangle final {
        vertex v0 = { { -0.5f, -0.5f, 0 } };
        vertex v1 = { { 0.5f, -0.5f, 0 } };
        vertex v2 = { { 0, 0.5f, 0 } };

        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    triangle triangle_init(u32& shader_program, u32& vao, u32& vbo);
    void triangle_free(u32 shader_program, u32 vao, u32 vbo);

    typedef triangle (*triangle_factory_fn_t)(u32);

    std::vector<triangle> triangles_init(
            u32& shader_program,
            u32& vao,
            u32& vbo,
            u32 count,
            triangle_factory_fn_t triangle_factory_fn = [](u32 i) { return triangle(); }
    );

    struct rect_vertices final {
        vertex v0 = { { 0.5f, 0.5f, 0 } };
        vertex v1 = { { 0.5f, -0.5f, 0 } };
        vertex v2 = { { -0.5f, -0.5f, 0 } };
        vertex v3 = { { -0.5, 0.5f, 0 } };

        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    struct rect final {
        rect_vertices vertices;
        u32 indices[6] = {
                0, 1, 3,
                1, 2, 3
        };
    };

    void rect_init(u32& shader_program, u32& vao, u32& vbo, u32& ibo, const rect& geometry = {});
    void rect_free(u32 shader_program, u32 vao, u32 vbo, u32 ibo);

}
