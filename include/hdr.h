#pragma once

#include <frame.h>
#include <shader.h>
#include <buffers.h>

namespace gl {

    struct HDR_Renderer final {
        Texture render_target;
        Texture src;
        Texture shiny;

        float exposure = 1.0f;
        float shiny_strength = 7.5f;

        void init(int w, int h);
        void free();

        void set_exposure(float exposure);
        void set_shiny_strength(float shiny_strength);

        void resize(int w, int h);

        void render();

    private:
        Shader shader;
        FrameBuffer fbo;
        VertexArray vao;
    };

}