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

        ColorAttachment color = { 0, w, h };
        color.image.internalFormat = GL_RGB;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::U8;
        color.init();

        mFrame.colors = { color };
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
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mShader.use();
        mShader.update();
        mDrawable.draw();
    }

    void BlurRenderer::updateOffset() {
        mShader.updateOffset();
    }

}