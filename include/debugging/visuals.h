#pragma once

#include <api/frame.h>

#include <ecs/entity.h>

#include <debugging/normal_visual.h>
#include <debugging/polygon_visual.h>
#include <debugging/light_visual.h>

namespace gl {

    struct VisualsPipeline final {
        Scene* scene;

        VisualsPipeline(Scene* scene, int width, int height);
        ~VisualsPipeline();

        inline const ImageBuffer& getRenderTarget() {
            return mRenderTarget;
        }

        void resize(int w, int h);

        void render();

    private:
        void initFrame(int w, int h);

    private:
        FrameBuffer mFrame;
        ImageBuffer mRenderTarget;
        PolygonVisualRenderer* mPolygonVisualRenderer;
        NormalVisualRenderer* mNormalVisualRenderer;
        LightVisualRenderer* mLightVisualRenderer;
    };

}