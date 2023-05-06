#pragma once

#include <api/shader.h>
#include <api/draw.h>
#include <api/frame.h>

namespace gl {

    struct BlurParams final {
        ImageBuffer sceneBuffer;
        UniformF offset = { "offset", 1.0f / 300.0f };
    };

    struct BlurShader : Shader {
        BlurParams params;

        void init();
        void update();

        void updateOffset();
    };

    struct BlurRenderer final {

        BlurRenderer(int w, int h);
        ~BlurRenderer();

        inline const ImageBuffer& getRenderTarget() {
            return mRenderTarget;
        }

        inline BlurParams& getParams() {
            return mShader.params;
        }

        void resize(int w, int h);

        void updateOffset();

        void render();

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        BlurShader mShader;
        DrawableQuad mDrawable;
    };

}