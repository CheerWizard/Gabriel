#pragma once

#include <triangle.h>
#include <rect.h>
#include <cube.h>

namespace gl {

    void free(u32 shader_program, u32 vao, u32 vbo);
    void free(u32 shader_program, u32 vao, u32 vbo, u32 ibo);

}
