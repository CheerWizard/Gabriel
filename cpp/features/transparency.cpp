#include <features/transparency.h>

namespace gl {

    void OITCompositeShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/oit_composite.frag");
        complete();
    }

    void OITCompositeShader::update() {
        bindSampler(params.accumSampler, params.accumBuffer);
        bindSampler(params.revealSampler, params.revealBuffer);
    }

    TransparentRenderer::TransparentRenderer(int width, int height) {
        mShader.init();
        mDrawable.init();

        initAccumRevealFrame(width, height);
        initCompositeFrame(width, height);
    }

    void TransparentRenderer::initAccumRevealFrame(int width, int height) {
        // setup accumulation
        ColorAttachment accum = { 0, width, height };
        accum.image.internalFormat = GL_RGBA16F;
        accum.image.pixelFormat = GL_RGBA;
        accum.image.pixelType = PixelType::FLOAT;
        accum.params.minFilter = GL_LINEAR;
        accum.params.magFilter = GL_LINEAR;
        accum.clearColor = COLOR_ZERO;
        accum.init();

        // setup alpha reveal
        ColorAttachment reveal = { 1, width, height };
        reveal.image.internalFormat = GL_R8;
        reveal.image.pixelFormat = GL_RED;
        reveal.image.pixelType = PixelType::U8;
        reveal.params.minFilter = GL_LINEAR;
        reveal.params.magFilter= GL_LINEAR;
        reveal.clearColor = COLOR_ONE;
        reveal.init();

        mFrame.colors = { accum, reveal };

        mFrame.rbo = { width, height };
        mFrame.rbo.init();

        mFrame.init();
        mFrame.attachColors();
        mFrame.attachRenderBuffer();
        mFrame.complete();

        mShader.params.accumBuffer = mFrame.colors[0].buffer;
        mShader.params.revealBuffer = mFrame.colors[1].buffer;
    }

    void TransparentRenderer::initCompositeFrame(int width, int height) {
        ColorAttachment composite = { 0, width, height };
        composite.image.internalFormat = GL_RGBA16F;
        composite.image.pixelFormat = GL_RGBA;
        composite.image.pixelType = PixelType::FLOAT;
        composite.params.minFilter = GL_LINEAR;
        composite.params.magFilter = GL_LINEAR;
        composite.init();

        mCompositeFrame.colors = { composite };

        mCompositeFrame.rbo = { width, height };
        mCompositeFrame.rbo.init();

        mCompositeFrame.init();
        mCompositeFrame.attachColors();
        mCompositeFrame.attachRenderBuffer();
        mCompositeFrame.complete();

        mRenderTarget = mCompositeFrame.colors[0].buffer;
    }

    TransparentRenderer::~TransparentRenderer() {
        mFrame.free();
        mCompositeFrame.free();
        mShader.free();
        mDrawable.free();
    }

    void TransparentRenderer::resize(int w, int h) {
        mFrame.resize(w, h);
        mCompositeFrame.resize(w, h);
    }

    void TransparentRenderer::bind() {
        mFrame.bind();
//        mFrame.clearColorBuffers();
//        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glDepthMask(GL_FALSE);

        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
    }

    void TransparentRenderer::unbind() {
        mCompositeFrame.bind();
//        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glDepthFunc(GL_ALWAYS);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mShader.use();
        mShader.update();
        mDrawable.draw();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void TransparentRenderer::blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const {
        FrameBuffer::blit(srcColorFrame, w, h, mFrame.id, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(srcDepthFrame, w, h, mFrame.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

}