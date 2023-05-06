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

    ScreenRenderer::ScreenRenderer() {
        mShader.init();
        mDrawable.init();
    }

    ScreenRenderer::~ScreenRenderer() {
        mShader.free();
        mDrawable.free();
    }

    void ScreenRenderer::render() {
        FrameBuffer::unbind();
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mShader.use();
        mShader.update();

        mDrawable.draw();
    }

    void ScreenRenderer::updateGamma() {
        mShader.updateGamma();
    }

}