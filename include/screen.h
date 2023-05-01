#pragma once

#include <shader.h>
#include <draw.h>

namespace gl {

    struct ScreenParams final {
        const ImageSampler scene_sampler = { "scene", 0 };
        ImageBuffer scene_buffer;

        const ImageSampler ui_sampler = { "ui", 1 };
        ImageBuffer ui_buffer;

        const ImageSampler debug_control_sampler = { "debug_control", 2 };
        ImageBuffer debug_control_buffer;

        UniformF gamma = { "gamma", 2.2f };
    };

    struct ScreenShader : Shader {
        ScreenParams params;

        void init();
        void update();

        void update_gamma();
    };

    struct ScreenRenderer final {

        ScreenRenderer();
        ~ScreenRenderer();

        inline ScreenParams& get_params() {
            return shader.params;
        }

        void update_gamma();

        void render();

    private:
        ScreenShader shader;
        DrawableQuad drawable;
    };

}
