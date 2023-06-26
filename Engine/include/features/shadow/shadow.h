#pragma once

#include <features/shadow/shadow_direct.h>
#include <features/shadow/shadow_point.h>

namespace gl {

    component(Shadowable) {};

    struct GABRIEL_API ShadowPipeline final {
        Scene* scene;

        DirectShadow directShadow;
        PointShadow pointShadow;

        ShadowPipeline(Scene* scene, int width, int height, Camera* camera);
        ~ShadowPipeline();

        void resize(int width, int height);

        void render();

    private:
        void createDirectShadow(int width, int height, Camera* camera);
        void createPointShadow(int width, int height, Camera* camera);

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