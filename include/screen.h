#pragma once

#include <buffers.h>
#include <shader.h>

namespace gl {

    struct Screen final {
        static ImageBuffer src;
        static float gamma;

        static void init();
        static void free();

        static void set_gamma(float gamma);

        static void render();

    private:
        static Shader shader;
        static VertexArray vao;
    };

}
