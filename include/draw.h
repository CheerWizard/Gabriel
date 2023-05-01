#pragma once

#include <buffers.h>
#include <shader.h>
#include <commands.h>
#include <component.h>

namespace gl {

    component(DrawableVertices) {
        u32 type = GL_TRIANGLES;
        VertexArray vao;
        VertexBuffer vbo;
        int vertex_count = 0;

        void free();

        void draw();
        void draw(int instances);
    };

    component(DrawableElements) {
        u32 type = GL_TRIANGLES;
        VertexArray vao;
        VertexBuffer vbo;
        IndexBuffer ibo;
        u32 strips = 1;
        int vertices_per_strip = 0;

        void free();

        void draw() const;
        void draw(int instances) const;
    };

    component(DrawableQuad) {

        void init();
        void free();

        void draw() const;

    private:
        VertexArray vao;
    };

}