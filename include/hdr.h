#pragma once

#include <frame.h>
#include <shader.h>
#include <draw.h>

namespace gl {

    struct HdrParams final {
        UniformF exposure = { "exposure", 1.0f };
        UniformF shiny_strength = { "shiny_strength", 7.5f };

        const ImageSampler scene_sampler = { "scene", 0 };
        ImageBuffer scene_buffer;

        const ImageSampler shiny_sampler = { "shiny", 1 };
        ImageBuffer shiny_buffer;
    };

    struct HdrShader : Shader {
        HdrParams params;

        void init();
        void update();

        void update_exposure();
        void update_shiny_strength();
    };

    struct HdrRenderer final {

        HdrRenderer(int w, int h);
        ~HdrRenderer();

        inline const ImageBuffer& get_render_target() {
            return render_target;
        }

        inline HdrParams& get_params() {
            return shader.params;
        }

        void resize(int w, int h);

        void render();

        void update_exposure();
        void update_shiny_strength();

    private:
        ImageBuffer render_target;
        FrameBuffer fbo;
        HdrShader shader;
        DrawableQuad drawable;
    };

}