#pragma once

#include <buffers.h>
#include <shader.h>
#include <commands.h>

namespace gl {

    void draw_triangle(u32 vao);
    void draw_triangles(u32 vao, int count);
    void draw_rect(u32 vao, u32 ibo);
    void draw_rects(u32 vao, u32 ibo, int count);
    void draw_cube(u32 vao, u32 ibo);
    void draw_cubes(u32 vao, u32 ibo, int count);

    struct drawable_vertices final {
        u32 vertex_count = 0;
        u32 vao = 0;
        u32 vbo = 0;
    };

    struct drawable_elements final {
        u32 index_count = 0;
        u32 vao = 0;
        u32 vbo = 0;
        u32 ibo = 0;
    };

    void drawable_free(const drawable_vertices& vertices);
    void drawable_free(const drawable_elements& elements);

    void draw(const drawable_vertices& vertices);
    void draw(const drawable_vertices& vertices, u32 instance_count);
    void draw(const drawable_elements& elements);
    void draw(const drawable_elements& elements, u32 instance_count);

    void draw_quad(u32 vao);

}