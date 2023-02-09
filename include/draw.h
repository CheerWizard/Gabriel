#pragma once

#include <buffers.h>
#include <shader.h>
#include <commands.h>

namespace gl {

    void draw_triangle(u32 shader_program, u32 vao);
    void draw_triangles(u32 shader_program, u32 vao, int count);
    void draw_rect(u32 shader_program, u32 vao, u32 ibo);
    void draw_rects(u32 shader_program, u32 vao, u32 ibo, int count);

}