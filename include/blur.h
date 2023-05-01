#pragma once

#include <shader.h>
#include <draw.h>
#include <frame.h>

namespace gl {

    struct BlurParams final {
        ImageBuffer scene_buffer;
        UniformF offset = { "offset", 1.0f / 300.0f };
    };

    struct BlurShader : Shader {
        BlurParams params;

        void init();
        void update();

        void update_offset();
    };

    struct BlurRenderer final {

        BlurRenderer(int w, int h);
        ~BlurRenderer();

        inline const ImageBuffer& get_render_target() {
            return render_target;
        }

        inline BlurParams& get_params() {
            return shader.params;
        }

        void resize(int w, int h);

        void update_offset();

        void render();

    private:
        ImageBuffer render_target;
        FrameBuffer fbo;
        BlurShader shader;
        DrawableQuad drawable;
    };

}