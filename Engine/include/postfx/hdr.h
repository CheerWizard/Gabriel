#pragma once

#include <api/frame.h>
#include <api/shader.h>
#include <api/draw.h>

namespace gl {

    struct GABRIEL_API HdrParams final {
        UniformF exposure = { "exposure", 1.0f };
        UniformF shinyStrength = {"shiny_strength", 7.5f };

        const ImageSampler sceneSampler = { "scene", 0 };
        ImageBuffer sceneBuffer;

        const ImageSampler shinySampler = { "shiny", 1 };
        ImageBuffer shinyBuffer;
    };

    struct GABRIEL_API HdrShader : Shader {
        HdrParams params;

        void init();
        void update();

        void updateExposure();
        void updateShinyStrength();
    };

    struct GABRIEL_API HdrRenderer final {
        bool isEnabled = false;

        HdrRenderer(int w, int h);
        ~HdrRenderer();

        inline const ImageBuffer& getRenderTarget() {
            return mRenderTarget;
        }

        inline HdrParams& getParams() {
            return mShader.params;
        }

        [[nodiscard]] inline const FrameBuffer& getColorFrame() const {
            return mFrame;
        }

        void resize(int w, int h);

        void render();

        void updateExposure();
        void updateShinyStrength();

    private:
        ColorAttachment initColor(int width, int height);

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        HdrShader mShader;
        DrawableQuad mDrawable;
    };

}