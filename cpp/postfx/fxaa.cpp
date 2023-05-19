#include <postfx/fxaa.h>

namespace gl {

    void FXAAShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/postfx/fxaa.frag");
        complete();
    }

    void FXAAShader::update() {
        params.srcBuffer.bindActivate(0);
        setUniform(params.inverseFilterSize);
        setUniform(params.spanMax);
        setUniform(params.reduceMin);
        setUniform(params.reduceMul);
    }

    FXAARenderer::FXAARenderer(int width, int height) {
        mShader.init();
        mDrawable.init();

        mFrame.colors = { initColor(width, height) };
        mFrame.init();
        mFrame.attachColors();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;

        mShader.params.inverseFilterSize.value = {
                1.0f / static_cast<float>(width),
                1.0f / static_cast<float>(height),
                0.0f
        };
    }

    FXAARenderer::~FXAARenderer() {
        mDrawable.free();
        mShader.free();
        mFrame.free();
    }

    void FXAARenderer::resize(int width, int height) {
        mFrame.resize(width, height);
        mShader.params.inverseFilterSize.value = {
                1.0f / static_cast<float>(width),
                1.0f / static_cast<float>(height),
                0.0f
        };
    }

    void FXAARenderer::render() {
        mFrame.bind();
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mShader.use();
        mShader.update();

        mDrawable.draw();
    }

    ColorAttachment FXAARenderer::initColor(int width, int height) {
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