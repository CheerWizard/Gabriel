#pragma once

#include <api/buffers.h>
#include <api/shader.h>
#include <api/commands.h>

#include <ecs/component.h>

namespace gl {

    component(DrawableVertices) {
        u32 type = GL_TRIANGLES;
        VertexArray vao;
        VertexBuffer vbo;
        int vertexCount = 0;

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