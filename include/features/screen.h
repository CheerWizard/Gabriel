#pragma once

#include <api/shader.h>
#include <api/draw.h>
#include <api/frame.h>

namespace gl {

    struct ScreenParams final {
        const ImageSampler sceneSampler = { "scene", 0 };
        ImageBuffer sceneBuffer;

        const ImageSampler uiSampler = { "ui", 1 };
        ImageBuffer uiBuffer;

        const ImageSampler visualsSampler = {"visuals", 2 };
        ImageBuffer visualsBuffer;

        UniformF gamma = { "gamma", 2.2f };
    };

    struct ScreenShader : Shader {
        ScreenParams params;

        void init();
        void update();

        void updateGamma();
    };

    struct ScreenRenderer final {

        ScreenRenderer();
        ~ScreenRenderer();

        inline ScreenParams& getParams() {
            return mShader.params;
        }

        void updateGamma();

        void render();

    private:
        ScreenShader mShader;
        DrawableQuad mDrawable;
    };

}
