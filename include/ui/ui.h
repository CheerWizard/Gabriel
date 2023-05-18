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

        void resize(int w, int h);

        void render();

    private:
        void initFrame(int w, int h);

    private:
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        Text2dRenderer* mText2dRenderer;
        Text3dRenderer* mText3dRenderer;
    };

}