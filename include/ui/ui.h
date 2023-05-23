#pragma once

#include <api/frame.h>

#include <text/text.h>

namespace gl {

    struct UI_Pipeline final {
        Scene* scene;

        UI_Pipeline(Scene* scene, int width, int height);
        ~UI_Pipeline();

        inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        [[nodiscard]] inline const FrameBuffer& getColorFrame() const {
            return mFrame;
        }

        [[nodiscard]] inline const FrameBuffer& getDepthFrame() const {
            return mFrame;
        }

        void resize(int w, int h);

        void render();

        void blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const;

    private:
        void initFrame(int w, int h);

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        Text2dRenderer* mText2dRenderer;
        Text3dRenderer* mText3dRenderer;
    };

}