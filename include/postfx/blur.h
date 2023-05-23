#pragma once

#include <api/shader.h>
#include <api/draw.h>
#include <api/frame.h>

namespace gl {

    struct BlurParams final {
        ImageBuffer sceneBuffer;
        UniformF offset = { "offset", 0.003f };
    };

    struct BlurShader : Shader {
        BlurParams params;

        void init();
        void update();

        void updateOffset();
    };

    struct BlurRenderer final {
        bool isEnabled = false;

        BlurRenderer(int w, int h);
        ~BlurRenderer();

        inline const ImageBuffer& getRenderTarget() {
            return mRenderTarget;
        }

        inline BlurParams& getParams() {
            return mShader.params;
        }

        [[nodiscard]] inline const FrameBuffer& getColorFrame() const {
            return mFrame;
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