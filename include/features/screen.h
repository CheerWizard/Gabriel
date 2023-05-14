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

        const ImageSampler visualsSampler = { "visuals", 2 };
        ImageBuffer visualsBuffer;

        const ImageSampler raytraceSampler = { "raytrace", 3 };
        ImageBuffer raytraceBuffer;

        UniformF gamma = { "gamma", 2.2f };
    };

    struct ScreenShader : Shader {
        ScreenParams params;

        void init();
        void update();

        void updateGamma();
    };

    struct ScreenRenderer final {

        ScreenRenderer(int width, int height);
        ~ScreenRenderer();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        inline ScreenParams& getParams() {
            return mShader.params;
        }

        void resize(int width, int height);

        void updateGamma();

        void render();
        void renderBackBuffer();

    private:
        void renderInternal();

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        ScreenShader mShader;
        DrawableQuad mDrawable;
    };

}
