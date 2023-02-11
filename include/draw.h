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

}