#include <postfx/ssao.h>

#include "math/maths.h"

#include <random>

namespace gl {

    void SsaoShader::init(int width, int height) {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/ssao.frag");
        complete();
        params.resolution.value = { width, height };
        params.init();
    }

    void SsaoShader::update() {
        bindSampler(params.positionsSampler, params.positions);
        bindSampler(params.normalsSampler, params.normals);
        bindSampler(params.noiseSampler, params.noise);

        setUniform(params.resolution);
        setUniform(params.noiseSize);

        setUniformArray(params.samples);

        params.samplesSize.value = (int) params.samples.size();
        setUniform(params.samplesSize);
        setUniform(params.sampleRadius);
        setUniform(params.sampleBias);

        setUniform(params.occlusionPower);
    }

    void SsaoShader::resize(int width, int height) {
        params.resolution.value = { width, height };
    }

    void SsaoBlurShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/ssao_blur.frag");
        complete();
    }

    void SsaoBlurShader::update(const ImageBuffer &ssaoImage) {
        ssaoImage.activate(0);
        ssaoImage.bind();
    }

    SsaoRenderer::SsaoRenderer(int width, int height) {
        mSsaoShader.init(width, height);
        mBlurShader.init();

        mDrawable.init();

        // SSAO frame
        ColorAttachment color = { 0, width, height };
        color.image.internalFormat = GL_RED;
        color.image.pixelFormat = GL_RED;
        color.image.pixelType = PixelType::FLOAT;
        color.params.minFilter = GL_NEAREST;
        color.params.magFilter = GL_NEAREST;
        color.params.s = GL_REPEAT;
        color.params.t = GL_REPEAT;
        color.params.r = GL_REPEAT;
        color.init();

        mFrame.colors = { color };
        mFrame.init();
        mFrame.attachColors();
        mFrame.complete();

        // Blur frame
        ColorAttachment blur_color = { 0, width, height };
        blur_color.image.internalFormat = GL_RED;
        blur_color.image.pixelFormat = GL_RED;
        blur_color.image.pixelType = PixelType::FLOAT;
        blur_color.params.minFilter = GL_NEAREST;
        blur_color.params.magFilter = GL_NEAREST;
        blur_color.params.s = GL_REPEAT;
        blur_color.params.t = GL_REPEAT;
        blur_color.params.r = GL_REPEAT;
        blur_color.init();

        mBlurFrame.colors = { blur_color };
        mBlurFrame.init();
        mBlurFrame.attachColors();
        mBlurFrame.complete();

        mSsaoImage = mFrame.colors[0].buffer;
        mRenderTarget = mBlurFrame.colors[0].buffer;
    }

    SsaoRenderer::~SsaoRenderer() {
        mFrame.free();
        mSsaoShader.free();
        mSsaoShader.params.free();
        mBlurFrame.free();
        mBlurShader.free();
        mDrawable.free();
    }

    void SsaoRenderer::render() {
        // Occlusion part
        mFrame.bind();
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mSsaoShader.use();
        mSsaoShader.update();

        mDrawable.draw();

        // Blur part
        mBlurFrame.bind();
        FrameBuffer::clearBuffer(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mBlurShader.use();
        mBlurShader.update(mSsaoImage);

        mDrawable.draw();
    }

    void SsaoRenderer::resize(int w, int h) {
        mFrame.resize(w, h);
        mBlurFrame.resize(w, h);
        mSsaoShader.resize(w, h);
    }

    void SsaoParams::init() {
        // setup kernels
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        int samplesSize = (int) samples.size();
        for (u32 i = 0; i < samplesSize; i++) {
            glm::vec3 sample = {
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator)
            };
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = (float) i / samplesSize;
            scale = gl::lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            samples[i] = sample;
        }
        // setup noise
        std::vector<glm::vec3> noises(noiseSize.value * noiseSize.value);
        for (u32 i = 0; i < noiseSize.value * noiseSize.value; i++) {
            noises[i] = {
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    0
            };
        }
        // setup noise texture
        ImageParams noiseParams;
        noiseParams.minFilter = GL_NEAREST;
        noiseParams.magFilter = GL_NEAREST;
        noiseParams.s = GL_REPEAT;
        noiseParams.t = GL_REPEAT;
        noiseParams.r = GL_REPEAT;
        noise.init();
        Image noiseImage = { noiseSize.value, noiseSize.value, 3, GL_RGBA32F, GL_RGB, PixelType::FLOAT, &noises[0] };
        noise.load(noiseImage, noiseParams);
    }

    void SsaoParams::free() {
        noise.free();
    }

}