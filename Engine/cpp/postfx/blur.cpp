#include <postfx/blur.h>

namespace gl {

    void BlurShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/blur.frag");
        complete();
        updateOffset();
    }

    void BlurShader::update() {
        params.sceneBuffer.activate(0);
        params.sceneBuffer.bind();
    }

    void BlurShader::updateOffset() {
        use();
        setUniform(params.offset);
    }

    BlurRenderer::BlurRenderer(int w, int h) {
        mShader.init();
        mDrawable.init();

        mFrame.colors = { initColor(w, h) };
        mFrame.init();
        mFrame.attachColors();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;
    }

    BlurRenderer::~BlurRenderer() {
        mFrame.free();
        mShader.free();
        mDrawable.free();
    }

    void BlurRenderer::resize(int w, int h) {
        mFrame.resize(w, h);
    }

    void BlurRenderer::render() {
        mFrame.bind();
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mShader.use();
        mShader.update();
        mDrawable.draw();
    }

    void BlurRenderer::updateOffset() {
        mShader.updateOffset();
    }

    ColorAttachment BlurRenderer::initColor(int width, int height) {
        ColorAttachment color = { 0, width, height };
        color.image.internalFormat = GL_RGB;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::U8;
        color.init();
        return color;
    }

}