#pragma once

#include <scene.h>
#include <shadow_direct.h>
#include <shadow_point.h>

namespace gl {

    component(Shadowable) {};

    struct ShadowPipeline final {
        ecs::Scene* scene;

        DirectShadow directShadow;
        PointShadow pointShadow;

        ShadowPipeline(ecs::Scene* scene, int width, int height);
        ~ShadowPipeline();

        void resize(int width, int height);

        void render();

    private:
        void createDirectShadow(int width, int height);
        void createPointShadow(int width, int height);

        void begin();
        void end();

        void bind(const DepthAttachment& shadowMap);

        void renderDirectShadows();
        void renderPointShadows();

    private:
        FrameBuffer mFrame;
        DirectShadowRenderer* mDirectShadowRenderer;
        PointShadowRenderer* mPointShadowRenderer;
    };

}