#include <features/transparency.h>

namespace gl {

    TransparentRenderer::TransparentRenderer(int w, int h) {
        mShader.addVertexStage("shaders/pbr.vert");
        mShader.addFragmentStage("shaders/pbr.frag");
        mShader.complete();

        mCompositeShader.addVertexStage("shaders/fullscreen_quad.vert");
        mCompositeShader.addFragmentStage("shaders/oit_composite.frag");
        mCompositeShader.complete();

        // setup accumulation
        ColorAttachment sceneAccum = {0, w, h };
        sceneAccum.image.internalFormat = GL_RGBA16F;
        sceneAccum.image.pixelFormat = GL_RGBA;
        sceneAccum.image.pixelType = PixelType::FLOAT;
        sceneAccum.params.minFilter = GL_LINEAR;
        sceneAccum.params.magFilter = GL_LINEAR;
        sceneAccum.init();
        // setup alpha revealage
        ColorAttachment sceneReveal = {1, w, h };
        sceneReveal.image.internalFormat = GL_R8;
        sceneReveal.image.pixelFormat = GL_RED;
        sceneReveal.image.pixelType = PixelType::FLOAT;
        sceneReveal.params.minFilter = GL_LINEAR;
        sceneReveal.params.magFilter= GL_LINEAR;
        sceneReveal.init();

        mFrame.colors = { sceneAccum, sceneReveal };

        mFrame.rbo = {w, h };
        mFrame.rbo.init();

        mFrame.init();
        mFrame.attachColors();
        mFrame.attachRenderBuffer();
        mFrame.complete();

        // setup composite color
        ColorAttachment compositeColor = sceneAccum;
        compositeColor.init();
        mCompositeFrame.colors = {compositeColor };

        mCompositeFrame.rbo = {w, h };
        mCompositeFrame.rbo.init();

        mCompositeFrame.init();
        mCompositeFrame.attachColors();
        mCompositeFrame.attachRenderBuffer();
        mCompositeFrame.complete();

        mDrawable.init();

        mRenderTarget = mCompositeFrame.colors[0].buffer;
        mTransparentBuffer = {
                mFrame.colors[0].buffer,
                mFrame.colors[1].buffer,
        };
    }

    TransparentRenderer::~TransparentRenderer() {
        mCompositeShader.free();
        mFrame.free();
        mCompositeFrame.free();
        mDrawable.free();
    }

    void TransparentRenderer::resize(int w, int h) {
        mFrame.resize(w, h);
        mCompositeFrame.resize(w, h);
    }

    void TransparentRenderer::begin() {
        mFrame.bind();
        glDepthMask(GL_FALSE);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        static glm::vec4 COLOR_ZERO = glm::vec4(0);
        static glm::vec4 COLOR_ONE = glm::vec4(1);
        glClearBufferfv(GL_COLOR, 0, &COLOR_ZERO[0]);
        glClearBufferfv(GL_COLOR, 1, &COLOR_ONE[0]);

        mShader.use();
    }

    void TransparentRenderer::end() {
        mCompositeFrame.bind();
        glDepthFunc(GL_ALWAYS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mCompositeShader.use();
        mCompositeShader.bindSampler("accum", 0, mFrame.colors[0].buffer);
        mCompositeShader.bindSampler("reveal", 1, mFrame.colors[1].buffer);
        mDrawable.draw();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void TransparentRenderer::render(EntityID entityId, Transform& transform, DrawableElements& drawable) {
        mShader.setUniformArgs("entity_id", entityId);
        transform.update(mShader);
        drawable.draw();
    }

    void TransparentRenderer::render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material) {
        mShader.setUniformArgs("entity_id", entityId);
        transform.update(mShader);
        material.update(mShader, 0);
        drawable.draw();
    }

}