#pragma once

#include <api/draw.h>
#include <api/shader.h>
#include <api/frame.h>

namespace gl {

    struct GABRIEL_API SsaoParams final {
        const ImageSampler positionsSampler = { "v_positions", 0 };
        ImageBuffer positions;

        const ImageSampler normalsSampler = { "v_normals", 1 };
        ImageBuffer normals;

        const ImageSampler noiseSampler = { "noise", 2 };
        ImageBuffer noise;

        UniformI noiseSize = { "noise_size", 4 };

        UniformV2F resolution = { "resolution", { 512, 512 } };

        UniformArrayV3F samples = { "samples", 64 };
        UniformI samplesSize = { "samples_size", 64 };
        UniformF sampleRadius = { "sample_radius", 1 };
        UniformF sampleBias = { "sample_bias", 0.025f };

        UniformI occlusionPower = { "occlusion_power", 4 };

        void init();
        void free();
    };

    struct GABRIEL_API SsaoShader : Shader {
        SsaoParams params;

        void init(int width, int height);
        void update();

        void resize(int width, int height);
    };

    struct GABRIEL_API SsaoBlurShader : Shader {
        void init();
        void update(const ImageBuffer& ssaoImage);
    };

    struct GABRIEL_API SsaoRenderer final {
        bool isEnabled = false;

        SsaoRenderer(int width, int height);
        ~SsaoRenderer();

        inline SsaoParams& getParams() {
            return mSsaoShader.params;
        }

        inline const ImageBuffer& getRenderTarget() {
            return mRenderTarget;
        }

        void resize(int w, int h);

        void render();

    private:
        ImageBuffer mRenderTarget;
        ImageBuffer mSsaoImage;
        SsaoShader mSsaoShader;
        FrameBuffer mFrame;
        SsaoBlurShader mBlurShader;
        FrameBuffer mBlurFrame;
        DrawableQuad mDrawable;
    };

}