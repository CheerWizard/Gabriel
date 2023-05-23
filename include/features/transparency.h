#pragma once

#include <api/frame.h>
#include <api/draw.h>

#include <features/transform.h>
#include <features/material.h>

namespace gl {

    component(Opaque) {};
    component(Transparent) {};

    struct OITParams final {
        const ImageSampler accumSampler = { "accum", 0 };
        ImageBuffer accumBuffer;
        const ImageSampler revealSampler = { "reveal", 1 };
        ImageBuffer revealBuffer;
    };

    struct OITCompositeShader : Shader {
        OITParams params;

        void init();
        void update();
    };

    struct TransparentRenderer final {
        bool isEnabled = false;

        TransparentRenderer(int width, int height);
        ~TransparentRenderer();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        inline OITParams& getParams() {
            return mShader.params;
        }

        [[nodiscard]] inline const FrameBuffer& getColorFrame() const {
            return mCompositeFrame;
        }

        [[nodiscard]] inline const FrameBuffer& getDepthFrame() const {
            return mFrame;
        }

        void bind();
        void unbind();

        void resize(int w, int h);

        void blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const;

    private:
        void initAccumRevealFrame(int width, int height);
        void initCompositeFrame(int width, int height);

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        FrameBuffer mCompositeFrame;
        OITCompositeShader mShader;
        DrawableQuad mDrawable;
    };

}