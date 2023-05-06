#pragma once

#include <api/frame.h>
#include <api/draw.h>
#include <api/shader.h>

namespace gl {

    struct BloomUpsampleShader : Shader {
        UniformF filterRadius = { "filter_radius", 0.005f };

        void init();

        void updateFilterRadius();
    };

    struct BloomDownsampleShader : Shader {
        UniformV2I resolution = { "resolution", { 800, 600 } };
        UniformI mipLevel = { "mip_level", 1 };

        void init(const glm::ivec2& resolution);

        void updateResolution();
    };

    struct BloomMixShader : Shader {
        UniformF bloomStrength = { "bloom_strength", 0.02f };
        const ImageSampler hdrSampler = { "hdr", 0 };
        ImageBuffer hdrBuffer;
        const ImageSampler bloomSampler = { "bloom", 1 };
        ImageBuffer bloomBuffer;

        void init();
        void update();
        void updateBloomStrength();
    };

    struct BloomRenderer final {
        int mipLevels = 6;

        BloomRenderer(int width, int height);
        ~BloomRenderer();

        inline const ImageBuffer& getRenderTarget() {
            return mRenderTarget;
        }

        inline ImageBuffer& getHdrBuffer() {
            return mMixShader.hdrBuffer;
        }

        inline ImageBuffer& getBloomBuffer() {
            return mMixShader.bloomBuffer;
        }

        inline float& getFilterRadius() {
            return mUpsampleShader.filterRadius.value;
        }

        inline float& getBloomStrength() {
            return mMixShader.bloomStrength.value;
        }

        void resize(int w, int h);

        void render();

    private:
        void initMixColor();
        void initMips();

        void renderDownsample();
        void renderUpsample();
        void renderMix();

    private:
        glm::ivec2 mResolution;
        ImageBuffer mRenderTarget;
        FrameBuffer mFrame;
        BloomDownsampleShader mDownsampleShader;
        BloomUpsampleShader mUpsampleShader;
        BloomMixShader mMixShader;
        DrawableQuad mDrawable;
        std::vector<ColorAttachment> mMips;
        ColorAttachment mMixColor;
    };

}