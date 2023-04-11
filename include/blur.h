#pragma once

#include <shader.h>
#include <buffers.h>
#include <frame.h>

namespace gl {

    struct Blur final {
        static ImageBuffer src;
        static ImageBuffer render_target;
        static float offset;

        static void init(int w, int h);
        static void free();

        static void resize(int w, int h);

        static void render();

    private:
        static Shader shader;
        static FrameBuffer fbo;
        static VertexArray vao;
    };

}