#pragma once

#include <buffers.h>
#include <shader.h>
#include <commands.h>

namespace gl {

    struct DrawableVertices {
        int vertex_count = 0;
        VertexArray vao;
        VertexBuffer vbo;
        u32 type = GL_TRIANGLES;

        void free();

        void draw();
        void draw(int instances);
    };

    struct DrawableElements {
        int index_count;
        VertexArray vao;
        VertexBuffer vbo;
        IndexBuffer ibo;
        u32 type = GL_TRIANGLES;

        void free();

        void draw() const;
        void draw(int instances) const;
    };

}