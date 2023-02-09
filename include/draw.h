#pragma once

#include <buffers.h>
#include <shader.h>
#include <commands.h>

namespace gl {

    void draw_triangle(u32 shader_program, u32 vao);
    void draw_rect(u32 shader_program, u32 vao, u32 ibo);

}