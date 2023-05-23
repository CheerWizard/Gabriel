#pragma once

#include <api/buffers.h>
#include <api/shader.h>

namespace gl {

    enum DrawType : u32 {
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN = GL_TRIANGLE_FAN,
        LINES = GL_LINES,
        QUADS = GL_QUADS,
        POINTS = GL_POINTS
    };

    component(DrawableVertices) {
        DrawType type = DrawType::TRIANGLES;
        VertexArray vao;
        VertexBuffer vbo;
        int vertexCount = 0;

        void free();

        void draw();
        void draw(int instances);
    };

    component(DrawableElements) {
        DrawType type = DrawType::TRIANGLES;
        VertexArray vao;
        VertexBuffer vbo;
        IndexBuffer ibo;
        u32 strips = 1;
        int verticesPerStrip = 0;

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