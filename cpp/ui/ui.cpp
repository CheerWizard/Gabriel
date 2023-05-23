#include <ui/ui.h>

namespace gl {

    UI_Pipeline::UI_Pipeline(Scene* scene, int width, int height) : scene(scene) {
        initFrame(width, height);
        mText2dRenderer = new Text2dRenderer();
        mText3dRenderer = new Text3dRenderer();
    }

    UI_Pipeline::~UI_Pipeline() {
        mFrame.free();
        delete mText2dRenderer;
        delete mText3dRenderer;
    }

    void UI_Pipeline::resize(int w, int h) {
        mFrame.resize(w, h);
    }

    void UI_Pipeline::initFrame(int w, int h) {
        ColorAttachment uiColor = { 0, w, h };
        uiColor.image.internalFormat = GL_RGBA8;
        uiColor.image.pixelFormat = GL_RGBA;
        uiColor.image.pixelType = PixelType::U8;
        uiColor.params.minFilter = GL_LINEAR;
        uiColor.params.magFilter = GL_LINEAR;
        uiColor.init();

        mFrame.rbo = { w, h };
        mFrame.rbo.init();

        mFrame.colors = { uiColor };
        mFrame.init();
        mFrame.attachColors();
        mFrame.attachRenderBuffer();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;
    }

    void UI_Pipeline::render() {
        mFrame.bind();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mText2dRenderer->begin();
        scene->eachComponent<Text2d>([this](Text2d* text) {
            mText2dRenderer->render(*text);
        });

        mText3dRenderer->begin();
        scene->eachComponent<Text3d>([this](Text3d* text) {
            mText3dRenderer->render(*text);
        });
    }

    void UI_Pipeline::blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const {
        FrameBuffer::blit(srcColorFrame, w, h, mFrame.id, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(srcDepthFrame, w, h, mFrame.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

}