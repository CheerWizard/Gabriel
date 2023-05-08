#include <features/screen.h>

namespace gl {

    void ScreenShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/screen.frag");
        complete();
        updateGamma();
    }

    void ScreenShader::update() {
        bindSampler(params.sceneSampler, params.sceneBuffer);
        bindSampler(params.uiSampler, params.uiBuffer);
#ifdef DEBUG
        bindSampler(params.visualsSampler, params.visualsBuffer);
#endif
    }

    void ScreenShader::updateGamma() {
        use();
        setUniform(params.gamma);
    }

    ScreenRenderer::ScreenRenderer(int width, int height) {
        mShader.init();
        mDrawable.init();

        ColorAttachment color;
        color.image.internalFormat = GL_RGB;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::U8;
        color.image.width = width;
        color.image.height = height;
        color.init();

        mFrame.colors = { color };
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

    void ScreenRenderer::renderInternal() {
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);
        mShader.use();
        mShader.update();
        mDrawable.draw();
    }

}