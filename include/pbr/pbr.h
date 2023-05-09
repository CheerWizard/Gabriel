#pragma once

#include <api/buffers.h>

#include <ecs/entity.h>

#include <features/transform.h>
#include <features/material.h>
#include <features/light.h>
#include <features/outline.h>
#include <features/transparency.h>

#include <pbr/environment.h>

#include <animation/skeletal_renderer.h>

#include <shadow/shadow.h>

#include <postfx/ssao.h>

#include <terrain/terrain.h>

namespace gl {

    struct PBR_Pixel final {
        EntityID entityId = InvalidEntity;
    };

    component(PBR_Component_Forward) {};
    component(PBR_Component_Deferred) {};
    component(PBR_Component_ForwardCull) {};
    component(PBR_Component_DeferredCull) {};

    component(PBR_SkeletalComponent_Forward) {};
    component(PBR_SkeletalComponent_Deferred) {};
    component(PBR_SkeletalComponent_ForwardCull) {};
    component(PBR_SkeletalComponent_DeferredCull) {};

    struct PBR_Entity : Entity {

        PBR_Entity() : Entity() {}

        PBR_Entity(Scene* scene, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
        : Entity(scene) {
            addComponent<Transform>(translation, rotation, scale);
            addComponent<DrawableElements>();
            addComponent<Material>();
        }

        Transform* transform() {
            return scene->getComponent<Transform>(id);
        }

        DrawableElements* drawable() {
            return scene->getComponent<DrawableElements>(id);
        }

        Material* material() {
            return scene->getComponent<Material>(id);
        }

    };

    struct PBR_ForwardRenderer final {
        int samples = 1;

        DirectShadow* directShadow = null;
        PointShadow* pointShadow = null;

        PBR_ForwardRenderer(int width, int height);
        ~PBR_ForwardRenderer();

        inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        inline Shader& getShader() {
            return mShader;
        }

        inline FrameBuffer& getCurrentFrame() {
            return mCurrentFrame;
        }

        void resize(int w, int h);

        void setCameraPos(glm::vec3& camera_pos);
        void setSamples(int samples);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material);
        void render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& lightSpace);

        void update(Environment* env);

        void blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame);
        void blitEntityId(int w, int h, u32 srcFrame, int srcEntityId);

    private:
        ImageBuffer mRenderTarget;
        VertexArray mVao;
        Shader mShader;
        FrameBuffer mFrame;
        FrameBuffer mMsaaFrame;
        FrameBuffer mCurrentFrame;
    };

    struct PBR_GBuffer final {
        ImageBuffer position;
        ImageBuffer normal;
        ImageBuffer albedo;
        ImageBuffer pbrParams;
        ImageBuffer shadowProjCoords;
        ImageBuffer objectId;
        ImageBuffer viewPosition;
        ImageBuffer viewNormal;
    };

    struct PBR_DeferredRenderer final {
        int samples = 1;

        DirectShadow* directShadow = null;
        PointShadow* pointShadow = null;

        inline const ImageBuffer& getRenderTarget() const { return mRenderTarget; }
        inline const PBR_GBuffer& getGBuffer() const { return mGBuffer; }
        inline FrameBuffer& getGeometryFbo() { return mGeometryFrame; }
        inline FrameBuffer& getLightFbo() { return mLightFrame; }

        PBR_DeferredRenderer(int w, int h, SsaoRenderer* ssaoRenderer);
        ~PBR_DeferredRenderer();

        void resize(int w, int h);

        void setCameraPos(glm::vec3& camera_pos);
        void setSamples(int samples);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material);

        void update(Environment* env);

    private:
        ImageBuffer mRenderTarget;
        PBR_GBuffer mGBuffer;
        DrawableQuad mDrawable;
        Shader mGeometryShader;
        FrameBuffer mGeometryFrame;
        FrameBuffer mGeometryMsaaFrame;
        FrameBuffer mCurrentGeometryFrame;
        Shader mLightShader;
        FrameBuffer mLightFrame;
        SsaoRenderer* mSsaoRenderer;
    };

    struct PBR_Skeletal_ForwardRenderer : SkeletalRenderer {
        PBR_Skeletal_ForwardRenderer();
        void update(Environment* env);
    };

    struct PBR_Skeletal_DeferredRenderer : SkeletalRenderer {
        PBR_Skeletal_DeferredRenderer();
        void update(Environment* env);
    };

    struct PBR_Pipeline final {
        Scene* scene;
        Environment env;
        Terrain* terrain = null;

        PBR_Pipeline(Scene* scene, int width, int height, SsaoRenderer* ssaoRenderer);
        ~PBR_Pipeline();

        inline const ImageBuffer& getRenderTarget() const {
            return mPbrForwardRenderer->getRenderTarget();
        }

        inline const PBR_GBuffer& getGBuffer() const {
            return mPbrDeferredRenderer->getGBuffer();
        }

        inline const TransparentBuffer& getTransparentBuffer() const {
            return mTransparentRenderer->getTransparentBuffer();
        }

        inline void setDirectShadow(DirectShadow* directShadow) {
            mPbrForwardRenderer->directShadow = directShadow;
            mPbrDeferredRenderer->directShadow = directShadow;
        }

        inline void setPointShadow(PointShadow* pointShadow) {
            mPbrForwardRenderer->pointShadow = pointShadow;
            mPbrDeferredRenderer->pointShadow = pointShadow;
        }

        void setSamples(int samples);

        void initHdrEnv(const char* filepath, bool flipUV);
        void generateEnv();

        void resize(int width, int height);

        void setCameraPos(glm::vec3& cameraPos);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void render();

        void updateSunlight(DirectLightUniform& sunlight);
        void updatePointLights(std::array<PointLightUniform, 4>& pointLights);
        void updateFlashlight(SpotLightUniform& flashlight);

    private:
        void renderForward();
        void renderForwardDefault();
        void renderForwardCulling();
        void renderDeferred();

    private:
        glm::ivec2 mResolution;

        PBR_ForwardRenderer* mPbrForwardRenderer;
        PBR_DeferredRenderer* mPbrDeferredRenderer;

        PBR_Skeletal_ForwardRenderer* mSkeletalForwardRenderer;
        PBR_Skeletal_DeferredRenderer* mSkeletalDeferredRenderer;

        TransparentRenderer* mTransparentRenderer;

        OutlineRenderer* mOutlineRenderer;

        EnvRenderer* mEnvRenderer;

        UniformBuffer mSunlightUbo;
        UniformBuffer mLightsUbo;
        UniformBuffer mFlashlightUbo;
    };

}