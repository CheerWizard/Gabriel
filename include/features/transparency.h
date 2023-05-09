#pragma once

#include <api/frame.h>
#include <api/shader.h>
#include <api/draw.h>

#include <features/transform.h>
#include <features/material.h>

namespace gl {

    component(Transparency) {};

    struct TransparentBuffer final {
        ImageBuffer accumulation;
        ImageBuffer revealage;
    };

    struct TransparentRenderer final {

        TransparentRenderer(int width, int height);
        ~TransparentRenderer();

        inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        inline const TransparentBuffer& getTransparentBuffer() const {
            return mTransparentBuffer;
        }

        void begin();
        void end();

        void resize(int w, int h);

        void render(EntityID entityId, Transform& transform, DrawableElements& drawable);
        void render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material);

    private:
        ImageBuffer mRenderTarget;
        TransparentBuffer mTransparentBuffer;
        FrameBuffer mFrame;
        FrameBuffer mCompositeFrame;
        Shader mShader;
        Shader mCompositeShader;
        DrawableQuad mDrawable;
    };

}