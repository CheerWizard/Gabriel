#pragma once

#include <buffers.h>
#include <shader.h>
#include <commands.h>

namespace gl {

    struct drawable_vertices final {
        int vertex_count = 0;
        u32 vao = 0;
        u32 vbo = 0;
        u32 type = GL_TRIANGLES;
    };

    struct drawable_elements final {
        int index_count = 0;
        u32 vao = 0;
        u32 vbo = 0;
        u32 ibo = 0;
        u32 type = GL_TRIANGLES;
    };

    void drawable_free(const drawable_vertices& vertices);
    void drawable_free(const drawable_elements& elements);

    void draw(const drawable_vertices& vertices);
    void draw(const drawable_vertices& vertices, int instance_count);
    void draw(const drawable_elements& elements);
    void draw(const drawable_elements& elements, int instance_count);

    void draw_quad(u32 vao);

}