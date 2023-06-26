#include <pbr/pbr.h>

namespace gl {

    static std::array<ImageSampler, 7> samplers = {
            ImageSampler { "positions", 2 },
            ImageSampler { "normals", 3 },
            ImageSampler { "albedos", 4 },
            ImageSampler { "pbr_params", 5 },
            ImageSampler { "emissions", 6 },
            ImageSampler { "shadow_proj_coords", 7 },
            ImageSampler { "ssao", 8 }
    };

    PBR_Skeletal_ForwardRenderer::PBR_Skeletal_ForwardRenderer() {
        mShader.addVertexStage("shaders/skeletal.vert");
        mShader.addFragmentStage("shaders/pbr.frag");
        mShader.complete();
    }

    void PBR_Skeletal_ForwardRenderer::update(Environment* env) {
        mShader.use();
        mShader.setUniformStructArgs("envlight", "prefilter_levels", env->prefilterLevels);
        mShader.bindSamplerStruct("envlight", "irradiance", 16, env->irradiance);
        mShader.bindSamplerStruct("envlight", "prefilter", 17, env->prefilter);
        mShader.bindSamplerStruct("envlight", "brdf_convolution", 18, env->brdfConvolution);
    }

    PBR_Skeletal_DeferredRenderer::PBR_Skeletal_DeferredRenderer() {
        mShader.addVertexStage("shaders/skeletal_deferred.vert");
        mShader.addFragmentStage("shaders/pbr_material.frag");
        mShader.complete();
    }

    void PBR_Skeletal_DeferredRenderer::update(Environment* env) {
        mShader.use();
        mShader.setUniformStructArgs("envlight", "prefilter_levels", env->prefilterLevels);
        mShader.bindSamplerStruct("envlight", "irradiance", 16, env->irradiance);
        mShader.bindSamplerStruct("envlight", "prefilter", 17, env->prefilter);
        mShader.bindSamplerStruct("envlight", "brdf_convolution", 18, env->brdfConvolution);
    }

    PBR_ForwardRenderer::PBR_ForwardRenderer(int width, int height) {
        mShader.addVertexStage("shaders/pbr.vert");
        mShader.addFragmentStage("shaders/pbr.frag");
        mShader.complete();
        // setup VAO
        mVao.init();

        // setup PBR color
        ColorAttachment pbrColor = {0, width, height };
        pbrColor.image.internalFormat = GL_RGBA16F;
        pbrColor.image.pixelFormat = GL_RGBA;
        pbrColor.image.pixelType = PixelType::FLOAT;
        pbrColor.params.minFilter = GL_LINEAR;
        pbrColor.params.magFilter = GL_LINEAR;
        pbrColor.init();

        // setup PBR frame
        mFrame.colors = { pbrColor };
        mFrame.rbo = { width, height };
        mFrame.rbo.init();
        mFrame.init();
        mFrame.attachColors();
        mFrame.attachRenderBuffer();
        mFrame.complete();

        mRenderTarget = mFrame.colors[0].buffer;
    }

    PBR_ForwardRenderer::~PBR_ForwardRenderer() {
        mVao.free();
        mShader.free();
        mFrame.free();
    }

    void PBR_ForwardRenderer::resize(int w, int h) {
        mFrame.resize(w, h);
    }

    void PBR_ForwardRenderer::readPixel(PBR_Pixel& pixel, int x, int y) {
        mFrame.readPixel<PBR_Pixel>(pixel, 2, x, y);
    }

    void PBR_ForwardRenderer::bind() {
        mFrame.bind();
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }

    void PBR_ForwardRenderer::use() {
        mShader.use();
    }

    void PBR_ForwardRenderer::update(Environment* env) {
        mShader.use();
        mShader.setUniformStructArgs("envlight", "prefilter_levels", env->prefilterLevels);
        mShader.bindSamplerStruct("envlight", "irradiance", 16, env->irradiance);
        mShader.bindSamplerStruct("envlight", "prefilter", 17, env->prefilter);
        mShader.bindSamplerStruct("envlight", "brdf_convolution", 18, env->brdfConvolution);
    }

    void PBR_ForwardRenderer::render(Transform& transform, DrawableElements& drawable, Material& material) {
        if (!directShadow->lightSpaces.empty()) {
            mShader.bindSampler("direct_shadow_sampler", 0, directShadow->map.buffer);
            mShader.setUniformArgs("direct_light_space", directShadow->lightSpaces[0]);
            mShader.setUniformArgs("shadow_filter_size", directShadow->filterSize);
        }

//        m_shader.bindSampler("point_shadow_sampler", 1, pointShadow->map.buffer);
//        m_shader.setUniformArgs("far_plane", pointShadow->zFar);

        transform.update(mShader);
        material.update(mShader, 0);
        drawable.draw();
    }

    PBR_DeferredRenderer::PBR_DeferredRenderer(int width, int height, SsaoRenderer* ssaoRenderer)
    : mSsaoRenderer(ssaoRenderer) {
        mGeometryShader.addVertexStage("shaders/pbr_material.vert");
        mGeometryShader.addFragmentStage("shaders/pbr_material.frag");
        mGeometryShader.complete();

        mLightShader.addVertexStage("shaders/fullscreen_quad.vert");
        mLightShader.addFragmentStage("shaders/pbr_light.frag");
        mLightShader.complete();

        mDrawable.init();

        // setup PBR positions
        ColorAttachment pbrPos = { 0, width, height };
        pbrPos.image.internalFormat = GL_RGBA32F;
        pbrPos.image.pixelFormat = GL_RGBA;
        pbrPos.image.pixelType = PixelType::FLOAT;
        pbrPos.params.minFilter = GL_NEAREST;
        pbrPos.params.magFilter = GL_NEAREST;
        pbrPos.params.s = GL_CLAMP_TO_EDGE;
        pbrPos.params.t = GL_CLAMP_TO_EDGE;
        pbrPos.params.r = GL_CLAMP_TO_EDGE;
        pbrPos.init();

        // setup PBR normals
        ColorAttachment pbrNormal = { 1, width, height };
        pbrNormal.image.internalFormat = GL_RGBA32F;
        pbrNormal.image.pixelFormat = GL_RGBA;
        pbrNormal.image.pixelType = PixelType::FLOAT;
        pbrNormal.params.minFilter = GL_NEAREST;
        pbrNormal.params.magFilter= GL_NEAREST;
        pbrNormal.init();

        // setup PBR albedos
        ColorAttachment pbrAlbedo = { 2, width, height };
        pbrAlbedo.image.internalFormat = GL_RGBA;
        pbrAlbedo.image.pixelFormat = GL_RGBA;
        pbrAlbedo.image.pixelType = PixelType::U8;
        pbrAlbedo.params.minFilter = GL_NEAREST;
        pbrAlbedo.params.magFilter= GL_NEAREST;
        pbrAlbedo.init();

        // setup PBR params
        ColorAttachment pbrParams = { 3, width, height };
        pbrParams.image.internalFormat = GL_RGBA;
        pbrParams.image.pixelFormat = GL_RGBA;
        pbrParams.image.pixelType = PixelType::U8;
        pbrParams.params.minFilter = GL_NEAREST;
        pbrParams.params.magFilter= GL_NEAREST;
        pbrParams.init();

        // setup PBR emission
        ColorAttachment pbrEmission = { 4, width, height };
        pbrEmission.image.internalFormat = GL_RGB16F;
        pbrEmission.image.pixelFormat = GL_RGB;
        pbrEmission.image.pixelType = PixelType::FLOAT;
        pbrEmission.params.minFilter = GL_NEAREST;
        pbrEmission.params.magFilter = GL_NEAREST;
        pbrEmission.init();

        // setup PBR shadow projection coords
        ColorAttachment pbrShadowProj = { 5, width, height };
        pbrShadowProj.image.internalFormat = GL_RGBA32F;
        pbrShadowProj.image.pixelFormat = GL_RGBA;
        pbrShadowProj.image.pixelType = PixelType::FLOAT;
        pbrShadowProj.params.minFilter = GL_NEAREST;
        pbrShadowProj.params.magFilter= GL_NEAREST;
        pbrShadowProj.init();

        // setup PBR view positions
        ColorAttachment pbrViewPos = { 6, width, height };
        pbrViewPos.image.internalFormat = GL_RGBA32F;
        pbrViewPos.image.pixelFormat = GL_RGBA;
        pbrViewPos.image.pixelType = PixelType::FLOAT;
        pbrViewPos.params.minFilter = GL_NEAREST;
        pbrViewPos.params.magFilter = GL_NEAREST;
        pbrViewPos.params.s = GL_CLAMP_TO_EDGE;
        pbrViewPos.params.t = GL_CLAMP_TO_EDGE;
        pbrViewPos.params.r = GL_CLAMP_TO_EDGE;
        pbrViewPos.init();

        // setup PBR view normals
        ColorAttachment pbrViewNormal = {7, width, height };
        pbrViewNormal.image.internalFormat = GL_RGBA32F;
        pbrViewNormal.image.pixelFormat = GL_RGBA;
        pbrViewNormal.image.pixelType = PixelType::FLOAT;
        pbrViewNormal.params.minFilter = GL_NEAREST;
        pbrViewNormal.params.magFilter = GL_NEAREST;
        pbrViewNormal.init();

        // setup PBR material frame
        mGeometryFrame.colors = {
                pbrPos,
                pbrNormal,
                pbrAlbedo,
                pbrParams,
                pbrEmission,
                pbrShadowProj,
                pbrViewPos,
                pbrViewNormal
        };
        mGeometryFrame.rbo = { width, height };
        mGeometryFrame.rbo.init();
        mGeometryFrame.init();
        mGeometryFrame.attachColors();
        mGeometryFrame.attachRenderBuffer();
        mGeometryFrame.complete();

        // setup PBR light color
        ColorAttachment pbrLightColor = { 0, width, height };
        pbrLightColor.image.internalFormat = GL_RGBA16F;
        pbrLightColor.image.pixelFormat = GL_RGBA;
        pbrLightColor.image.pixelType = PixelType::FLOAT;
        pbrLightColor.params.minFilter = GL_LINEAR;
        pbrLightColor.params.magFilter = GL_LINEAR;
        pbrLightColor.init();

        // setup PBR light frame
        mLightFrame.colors = { pbrLightColor };
        mLightFrame.rbo = { width, height };
        mLightFrame.rbo.init();
        mLightFrame.init();
        mLightFrame.attachColors();
        mLightFrame.attachRenderBuffer();
        mLightFrame.complete();

        // PBR light pass upload
        {
            mLightShader.use();
            for (int i = 0 ; i < samplers.size() - 1 ; i++) {
                mLightShader.bindSampler(samplers[i], mGeometryFrame.colors[i].buffer);
            }
        }

        mRenderTarget = mLightFrame.colors[0].buffer;
        mGBuffer = {
                mGeometryFrame.colors[0].buffer,
                mGeometryFrame.colors[1].buffer,
                mGeometryFrame.colors[2].buffer,
                mGeometryFrame.colors[3].buffer,
                mGeometryFrame.colors[4].buffer,
                mGeometryFrame.colors[5].buffer,
                mGeometryFrame.colors[6].buffer,
                mGeometryFrame.colors[7].buffer
        };
    }

    PBR_DeferredRenderer::~PBR_DeferredRenderer() {
        mGeometryShader.free();
        mGeometryFrame.free();
        mLightShader.free();
        mLightFrame.free();
    }

    void PBR_DeferredRenderer::resize(int w, int h) {
        mGeometryFrame.resize(w, h);
        mLightFrame.resize(w, h);
    }

    void PBR_DeferredRenderer::readPixel(PBR_Pixel& pixel, int x, int y) {
        mGeometryFrame.readPixel<PBR_Pixel>(pixel, 5, x, y);
    }

    void PBR_DeferredRenderer::bind() {
        mGeometryFrame.bind();
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PBR_DeferredRenderer::unbind() {
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

        // render SSAO
        if (mSsaoRenderer->isEnabled) {
            mSsaoRenderer->getParams().positions = mGBuffer.viewPosition;
            mSsaoRenderer->getParams().normals = mGBuffer.viewNormal;
            mSsaoRenderer->render();
        }

        mLightFrame.bind();
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mLightShader.use();

        int samplers_size = samplers.size();
        mLightShader.bindSampler("direct_shadow_sampler", 0, directShadow->map.buffer);
        mLightShader.bindSampler("point_shadow_sampler", 1, pointShadow->map.buffer);
        mLightShader.setUniformArgs("far_plane", pointShadow->zFar);
        mLightShader.setUniformArgs("shadow_filter_size", directShadow->filterSize);

        for (int i = 0 ; i < samplers_size - 1 ; i++) {
            mLightShader.bindSampler(samplers[i], mGeometryFrame.colors[i].buffer);
        }

        if (mSsaoRenderer->isEnabled) {
            mLightShader.bindSampler(samplers[samplers_size - 1], mSsaoRenderer->getRenderTarget());
        }
        mLightShader.setUniformArgs("enable_ssao", mSsaoRenderer->isEnabled);

        mDrawable.draw();

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
    }

    void PBR_DeferredRenderer::use() {
        mGeometryShader.use();
    }

    void PBR_DeferredRenderer::render(Transform& transform, DrawableElements& drawable, Material& material) {
        if (!directShadow->lightSpaces.empty()) {
            mGeometryShader.setUniformArgs("direct_light_space", directShadow->lightSpaces[0]);
        }

        transform.update(mGeometryShader);
        material.update(mGeometryShader, 0);
        drawable.draw();
    }

    void PBR_DeferredRenderer::update(Environment* env) {
        mLightShader.use();
        mLightShader.setUniformStructArgs("envlight", "prefilter_levels", env->prefilterLevels);
        mLightShader.bindSamplerStruct("envlight", "irradiance", 16, env->irradiance);
        mLightShader.bindSamplerStruct("envlight", "prefilter", 17, env->prefilter);
        mLightShader.bindSamplerStruct("envlight", "brdf_convolution", 18, env->brdfConvolution);
    }

    void PBR_DeferredRenderer::blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const {
//        FrameBuffer::blit(srcColorFrame, w, h, mLightFrame.id, w, h, 1, GL_COLOR_BUFFER_BIT);
//        FrameBuffer::blit(srcDepthFrame, w, h, mGeometryFrame.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

    void PBR_ForwardRenderer::blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const {
        FrameBuffer::blit(srcColorFrame, w, h, mFrame.id, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(srcDepthFrame, w, h, mFrame.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

    PBR_Pipeline::PBR_Pipeline(
            Scene* scene,
            int width, int height,
            SsaoRenderer* ssaoRenderer,
            TransparentRenderer* transparentRenderer,
            EnvRenderer* envRenderer,
            OutlineRenderer* outlineRenderer
    ) : mResolution(width, height), scene(scene),
    mTransparentRenderer(transparentRenderer),
    mEnvRenderer(envRenderer),
    mOutlineRenderer(outlineRenderer) {

        mPbrForwardRenderer = new PBR_ForwardRenderer(width, height);
        mPbrDeferredRenderer = new PBR_DeferredRenderer(width, height, ssaoRenderer);
        mSkeletalForwardRenderer = new PBR_Skeletal_ForwardRenderer();
        mSkeletalDeferredRenderer = new PBR_Skeletal_DeferredRenderer();

    }

    PBR_Pipeline::~PBR_Pipeline() {
        delete mPbrForwardRenderer;
        delete mPbrDeferredRenderer;
        delete mSkeletalForwardRenderer;
        delete mSkeletalDeferredRenderer;
    }

    void PBR_Pipeline::setEnvironment(Environment* environment) {
        if (environment) {
            mEnvRenderer->setEnvironment(environment);
            mPbrForwardRenderer->use();
            environment->makeResident();
            mPbrDeferredRenderer->use();
            environment->makeResident();
        }
    }

    void PBR_Pipeline::resize(int width, int height) {
        mResolution = { width, height };
        mPbrForwardRenderer->resize(width, height);
        mPbrDeferredRenderer->resize(width, height);
    }

    void PBR_Pipeline::readPixel(PBR_Pixel& pixel, int x, int y) {
        mPbrForwardRenderer->readPixel(pixel, x, y);
    }

    void PBR_Pipeline::renderForward() {
        mPbrForwardRenderer->bind();

        glDisable(GL_CULL_FACE);
        mEnvRenderer->render();
        glEnable(GL_CULL_FACE);

        renderTransparent();
    }

    void PBR_Pipeline::renderTransparent() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mPbrForwardRenderer->use();

        scene->eachComponent<Transparent>([this](Transparent* transparent) {
            EntityID entityId = transparent->entityId;
            auto& transform = *scene->getComponent<Transform>(entityId);
            auto& drawable = *scene->getComponent<DrawableElements>(entityId);
            auto& material = *scene->getComponent<Material>(entityId);
            auto* outline = scene->getComponent<Outline>(entityId);

            if (outline) {
                mOutlineRenderer->unbind();

                mPbrForwardRenderer->render(transform, drawable, material);

                // render outline objects
                mOutlineRenderer->bind();
                mOutlineRenderer->use();
                mOutlineRenderer->render(*outline, transform, drawable);
                // reset renderer state
                mOutlineRenderer->unbind();
                mPbrForwardRenderer->use();
            } else {
                mPbrForwardRenderer->render(transform, drawable, material);
            }
        });
    }

    void PBR_Pipeline::renderDeferred() {
        mPbrDeferredRenderer->bind();

        mPbrDeferredRenderer->use();

        // render terrain
        if (terrain) {
            glCullFace(GL_FRONT);
            mPbrDeferredRenderer->render(
                    terrain->transform,
                    terrain->drawable,
                    terrain->material
            );
            glCullFace(GL_BACK);
        }

        // render scene
        scene->eachComponent<Opaque>([this](Opaque* opaque) {
            EntityID entityId = opaque->entityId;
            auto& transform = *scene->getComponent<Transform>(entityId);
            auto& drawable = *scene->getComponent<DrawableElements>(entityId);
            auto& material = *scene->getComponent<Material>(entityId);
            auto* outline = scene->getComponent<Outline>(entityId);

            if (outline) {
                mOutlineRenderer->unbind();

                mPbrDeferredRenderer->render(transform, drawable, material);

                // render outline objects
                mOutlineRenderer->bind();
                mOutlineRenderer->use();
                mOutlineRenderer->render(*outline, transform, drawable);
                // reset renderer state
                mOutlineRenderer->unbind();
                mPbrDeferredRenderer->use();
            } else {
                mPbrDeferredRenderer->render(transform, drawable, material);
            }
        });

        // todo handle skeletal animation rendering
//        mSkeletalDeferredRenderer->use();
//        scene->eachComponent<SkeletalComponent>([this](SkeletalComponent* component) {
//            EntityID entityId = component->entityId;
//            mSkeletalDeferredRenderer->render(
//                    *scene->getComponent<Transform>(entityId),
//                    *scene->getComponent<DrawableElements>(entityId),
//                    *scene->getComponent<Material>(entityId)
//            );
//        });

        mPbrDeferredRenderer->unbind();
    }

    void PBR_Pipeline::render() {
        renderDeferred();
        mPbrForwardRenderer->blitColorDepth(
                mResolution.x, mResolution.y,
                mPbrDeferredRenderer->getColorFrame().id,
                mPbrDeferredRenderer->getDepthFrame().id
        );
        renderForward();
    }

}