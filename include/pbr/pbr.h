#pragma once

#include <api/buffers.h>

#include <features/transform.h>
#include <features/material.h>
#include <features/lighting/light.h>
#include <features/outline.h>
#include <features/transparency.h>
#include <features/shadow/shadow.h>

#include <animation/skeletal_renderer.h>

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
        DirectShadow* directShadow = null;
        PointShadow* pointShadow = null;

        PBR_ForwardRenderer(int width, int height);
        ~PBR_ForwardRenderer();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        inline Shader& getShader() {
            return mShader;
        }

        void resize(int w, int h);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(Transform& transform, DrawableElements& drawable, Material& material);
        void render(Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& lightSpace);

        void update(Environment* env);

        void blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame);

    private:
        ImageBuffer mRenderTarget;
        VertexArray mVao;
        Shader mShader;
        FrameBuffer mFrame;
    };

    struct PBR_GBuffer final {
        ImageBuffer position;
        ImageBuffer normal;
        ImageBuffer albedo;
        ImageBuffer pbrParams;
        ImageBuffer emission;
        ImageBuffer shadowProjCoords;
        ImageBuffer viewPosition;
        ImageBuffer viewNormal;
    };

    struct PBR_DeferredRenderer final {
        DirectShadow* directShadow = null;
        PointShadow* pointShadow = null;

        inline const ImageBuffer& getRenderTarget() const { return mRenderTarget; }
        inline const PBR_GBuffer& getGBuffer() const { return mGBuffer; }
        inline FrameBuffer& getGeometryFbo() { return mGeometryFrame; }
        inline FrameBuffer& getLightFbo() { return mLightFrame; }

        PBR_DeferredRenderer(int w, int h, SsaoRenderer* ssaoRenderer);
        ~PBR_DeferredRenderer();

        void resize(int w, int h);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(Transform& transform, DrawableElements& drawable, Material& material);

        void update(Environment* env);

    private:
        ImageBuffer mRenderTarget;
        PBR_GBuffer mGBuffer;
        DrawableQuad mDrawable;
        FrameBuffer mGeometryFrame;
        Shader mGeometryShader;
        FrameBuffer mLightFrame;
        Shader mLightShader;
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
        Terrain* terrain = null;

        PBR_Pipeline(Scene* scene, int width, int height, SsaoRenderer* ssaoRenderer);
        ~PBR_Pipeline();

        inline const ImageBuffer& getRenderTarget() const {
            return mPbrForwardRenderer->getRenderTarget();
        }

        [[nodiscard]] inline const PBR_GBuffer& getGBuffer() const {
            return mPbrDeferredRenderer->getGBuffer();
        }

        [[nodiscard]] inline const TransparentBuffer& getTransparentBuffer() const {
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

        void setEnvironment(Environment* environment);

        void generateEnv();

        void resize(int width, int height);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void render();

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
    };

}