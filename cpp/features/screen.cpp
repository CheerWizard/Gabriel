#include <features/screen.h>

namespace gl {

    void ScreenShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/screen.frag");
        complete();
        updateGamma();
    }

    void ScreenShader::update() {
        params.buffer.bindActivate(0);
        setUniform(params.gamma);
    }

    void ScreenShader::updateGamma() {
        use();
        setUniform(params.gamma);
    }

    ScreenRenderer::ScreenRenderer(int width, int height) {
        mShader.init();
        mDrawable.init();

        mFrame.colors = { initColor(width, height) };
        mFrame.init();
        mFrame.attachColors();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;
    }

    ScreenRenderer::~ScreenRenderer() {
        mShader.free();
        mDrawable.free();
        mFrame.free();
    }

    void ScreenRenderer::resize(int width, int height) {
        mFrame.resize(width, height);
    }

    void ScreenRenderer::updateGamma() {
        mShader.updateGamma();
    }

    void ScreenRenderer::renderBackBuffer() {
        FrameBuffer::unbind();
        renderInternal();
    }

    void ScreenRenderer::render() {
        mFrame.bind();
        renderInternal();
        FrameBuffer::unbind();
    }

    ColorAttachment ScreenRenderer::initColor(int width, int height) {
        ColorAttachment color;

        // filters
        color.params.minFilter = GL_LINEAR;
        color.params.magFilter = GL_LINEAR;

        // data
        color.image.width = width;
        color.image.height = height;
        color.image.internalFormat = GL_RGB16F;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::FLOAT;

        color.init();

        return color;
    }

    void ScreenRenderer::renderInternal() {
        glEnable(GL_DEPTH_TEST);
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        mShader.use();
        mShader.update();
        mDrawable.draw();
    }

}