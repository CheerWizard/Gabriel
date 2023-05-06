#include <pbr/hdr.h>

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

    HdrRenderer::HdrRenderer(int w, int h) {
        mShader.init();
        mDrawable.init();

        ColorAttachment color;
        color.image.internalFormat = GL_RGB;
        color.image.pixelFormat = GL_RGB;
        color.image.pixelType = PixelType::U8;
        color.image.width = w;
        color.image.height = h;
        color.init();

        mFrame.colors = { color };
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
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

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

}