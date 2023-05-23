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

        [[nodiscard]] inline const FrameBuffer& getColorFrame() const {
            return mFrame;
        }

        [[nodiscard]] inline const FrameBuffer& getDepthFrame() const {
            return mFrame;
        }

        void resize(int w, int h);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void bind();

        void use();

        void render(Transform& transform, DrawableElements& drawable, Material& material);

        void update(Environment* env);

        void blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const;

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

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const { return mRenderTarget; }
        [[nodiscard]] inline const PBR_GBuffer& getGBuffer() const { return mGBuffer; }
        [[nodiscard]] inline const FrameBuffer& getColorFrame() const { return mLightFrame; }
        [[nodiscard]] inline const FrameBuffer& getDepthFrame() const { return mGeometryFrame; }

        PBR_DeferredRenderer(int w, int h, SsaoRenderer* ssaoRenderer);
        ~PBR_DeferredRenderer();

        void resize(int w, int h);

        void readPixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void use();

        void render(Transform& transform, DrawableElements& drawable, Material& material);

        void update(Environment* env);

        void blitColorDepth(int w, int h, u32 srcColorFrame, u32 srcDepthFrame) const;

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

        PBR_Pipeline(Scene* scene, int width, int height, SsaoRenderer* ssaoRenderer, TransparentRenderer* transparentRenderer);
        ~PBR_Pipeline();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return mPbrForwardRenderer->getRenderTarget();
        }

        [[nodiscard]] inline const PBR_GBuffer& getGBuffer() const {
            return mPbrDeferredRenderer->getGBuffer();
        }

        [[nodiscard]] inline const FrameBuffer& getColorFrame() const {
            return mPbrForwardRenderer->getColorFrame();
        }

        [[nodiscard]] inline const FrameBuffer& getDepthFrame() const {
            return mPbrForwardRenderer->getDepthFrame();
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
        void renderTransparent();
        void renderForward();
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