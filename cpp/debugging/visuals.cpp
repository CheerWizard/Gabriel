#include <debugging/visuals.h>

namespace gl {

    VisualsPipeline::VisualsPipeline(Scene* scene, int width, int height) : scene(scene) {
        initFrame(width, height);
        mPolygonVisualRenderer = new PolygonVisualRenderer();
        mNormalVisualRenderer = new NormalVisualRenderer();
        mLightVisualRenderer = new LightVisualRenderer();
    }

    void VisualsPipeline::initFrame(int width, int height) {
        ColorAttachment color = { 0, width, height };
        color.image.internalFormat = GL_RGB;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::U8;
        color.params.minFilter = GL_LINEAR;
        color.params.magFilter = GL_LINEAR;
        color.init();

        mFrame.colors = { color };
        mFrame.rbo = { width, height };
        mFrame.rbo.init();

        mFrame.init();
        mFrame.attachColors();
        mFrame.attachRenderBuffer();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;
    }

    VisualsPipeline::~VisualsPipeline() {
        mFrame.free();
        delete mPolygonVisualRenderer;
        delete mNormalVisualRenderer;
        delete mLightVisualRenderer;
    }

    void VisualsPipeline::resize(int w, int h) {
        mFrame.resize(w, h);
    }

    void VisualsPipeline::render() {
        mFrame.bind();

        // render visual drawables
        mPolygonVisualRenderer->begin();

        scene->eachComponent<Drawable2dVisual>([this](Drawable2dVisual* drawableVisual) {
            mPolygonVisualRenderer->render(*drawableVisual);
        });

        scene->eachComponent<Drawable3dVisual>([this](Drawable3dVisual* drawableVisual) {
            mPolygonVisualRenderer->render(*drawableVisual);
        });

        scene->eachComponent<Text2dVisual>([this](Text2dVisual* textVisual) {
            mPolygonVisualRenderer->render(*textVisual);
        });

        scene->eachComponent<Text3dVisual>([this](Text3dVisual* textVisual) {
            mPolygonVisualRenderer->render(*textVisual);
        });

        mPolygonVisualRenderer->end();

        // render visual normals
        mNormalVisualRenderer->begin();
        scene->eachComponent<NormalVisual>([this](NormalVisual* normalVisual) {
            EntityID entity_id = normalVisual->entityId;
            mNormalVisualRenderer->render(
                    *normalVisual,
                    *scene->getComponent<Transform>(entity_id),
                    *scene->getComponent<DrawableElements>(entity_id)
            );
        });

        // render visual lights
        mLightVisualRenderer->begin();
        scene->eachComponent<LightVisual>([this](LightVisual* lightVisual) {
            mLightVisualRenderer->render(*lightVisual);
        });
    }

    void VisualsPipeline::blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const {
        FrameBuffer::blit(srcColorFrame, w, h, mFrame.id, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(srcDepthFrame, w, h, mFrame.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

}