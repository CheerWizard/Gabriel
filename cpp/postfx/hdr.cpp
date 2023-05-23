#include <postfx/hdr.h>

namespace gl {

    void HdrShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/hdr.frag");
        complete();
        updateExposure();
        updateShinyStrength();
    }

    void HdrShader::update() {
        bindSampler(params.sceneSampler, params.sceneBuffer);
        bindSampler(params.shinySampler, params.shinyBuffer);
    }

    void HdrShader::updateExposure() {
        use();
        setUniform(params.exposure);
    }

    void HdrShader::updateShinyStrength() {
        use();
        setUniform(params.shinyStrength);
    }

    HdrRenderer::HdrRenderer(int width, int height) {
        mShader.init();
        mDrawable.init();

        mFrame.colors = { initColor(width, height) };
        mFrame.init();
        mFrame.attachColors();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;
    }

    HdrRenderer::~HdrRenderer() {
        mDrawable.free();
        mShader.free();
        mFrame.free();
        mShader.params.shinyBuffer.free();
    }

    void HdrRenderer::resize(int w, int h) {
        mFrame.resize(w, h);
    }

    void HdrRenderer::render() {
        mFrame.bind();
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mShader.use();
        mShader.update();

        mDrawable.draw();
    }

    void HdrRenderer::updateExposure() {
        mShader.updateExposure();
    }

    void HdrRenderer::updateShinyStrength() {
        mShader.updateShinyStrength();
    }

    ColorAttachment HdrRenderer::initColor(int width, int height) {
        ColorAttachment color;

        // data
        color.image.width = width;
        color.image.height = height;
        color.image.internalFormat = GL_RGB16F;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::FLOAT;

        // filter
        color.params.minFilter = GL_LINEAR;
        color.params.magFilter = GL_LINEAR;

        color.init();

        return color;
    }

}