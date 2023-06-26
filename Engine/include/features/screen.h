#pragma once

#include <api/draw.h>
#include <api/frame.h>

namespace gl {

    struct GABRIEL_API ScreenParams final {
        ImageBuffer buffer;
        UniformF gamma = { "gamma", 2.2f };
    };

    struct GABRIEL_API ScreenShader : Shader {
        ScreenParams params;

        void init();
        void update();

        void updateGamma();
    };

    struct GABRIEL_API ScreenRenderer final {

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
        ColorAttachment initColor(int width, int height);
        void renderInternal();

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        ScreenShader mShader;
        DrawableQuad mDrawable;
    };

}