#pragma once

#include <api/frame.h>
#include <api/draw.h>
#include <api/shader.h>

namespace gl {

    struct FXAAParams final {
        UniformV3F inverseFilterSize = { "inverseFilterSize", { 0, 0, 0 } };
        UniformF spanMax = { "spanMax", 8.0 };
        UniformF reduceMin = { "reduceMin", 1.0 / 128.0 };
        UniformF reduceMul = { "reduceMul", 1.0 / 8.0 };
        ImageBuffer srcBuffer;
    };

    struct FXAAShader : Shader {
        FXAAParams params;

        void init();
        void update();
    };

    struct FXAARenderer final {
        bool isEnabled = false;

        FXAARenderer(int width, int height);
        ~FXAARenderer();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        inline FXAAParams& getParams() {
            return mShader.params;
        }

        void resize(int width, int height);

        void render();

    private:
        ColorAttachment initColor(int width, int height);

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        DrawableQuad mDrawable;
        FXAAShader mShader;
    };

}