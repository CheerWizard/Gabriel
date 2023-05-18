#include <postfx/bloom.h>

namespace gl {

    void BloomUpsampleShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/bloom/bloom_upsample.frag");
        complete();
        updateFilterRadius();
    }

    void BloomUpsampleShader::updateFilterRadius() {
        use();
        setUniform(filterRadius);
    }

    void BloomUpsampleShader::update() {
        setUniform(filterRadius);
    }

    void BloomDownsampleShader::init(const glm::ivec2& resolution) {
        this->resolution.value = resolution;
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/bloom/bloom_downsample.frag");
        complete();
        updateResolution();
    }

    void BloomDownsampleShader::updateResolution() {
        use();
        setUniform(resolution);
    }

    void BloomMixShader::init() {
        addVertexStage("shaders/fullscreen_quad.vert");
        addFragmentStage("shaders/bloom/bloom_mix.frag");
        complete();
        updateBloomStrength();
    }

    void BloomMixShader::updateBloomStrength() {
        use();
        setUniform(bloomStrength);
    }

    void BloomMixShader::update() {
        bindSampler(hdrSampler, hdrBuffer);
        bindSampler(bloomSampler, bloomBuffer);
        setUniform(bloomStrength);
    }

    BloomRenderer::BloomRenderer(int width, int height) {
        mResolution = { width, height };

        mDownsampleShader.init(mResolution);
        mUpsampleShader.init();
        mMixShader.init();

        mDrawable.init();

        initMixColor();
        initMips();
        mFrame.colors = {mMips[0] };
        mFrame.init();
        mFrame.attachColors();
        mFrame.complete();

        mMixShader.bloomBuffer = mFrame.colors[0].buffer;
        mRenderTarget = mMixColor.buffer;
    }

    BloomRenderer::~BloomRenderer() {
        mFrame.free();
        mDownsampleShader.free();
        mUpsampleShader.free();
        mMixShader.free();
        mDrawable.free();

        mMixColor.free();
        for (auto& mip : mMips) {
            mip.free();
        }
    }

    void BloomRenderer::initMixColor() {
        // filter
        mMixColor.params.s = GL_CLAMP_TO_EDGE;
        mMixColor.params.t = GL_CLAMP_TO_EDGE;
        mMixColor.params.r = GL_CLAMP_TO_EDGE;
        mMixColor.params.minFilter = GL_LINEAR;
        mMixColor.params.magFilter = GL_LINEAR;
        // data
        mMixColor.image.internalFormat = GL_RGB16F;
        mMixColor.image.pixelFormat = GL_RGB;
        mMixColor.image.pixelType = PixelType::FLOAT;
        mMixColor.image.width = mResolution.x;
        mMixColor.image.height = mResolution.y;

        mMixColor.init();
    }

    void BloomRenderer::initMips() {
        if (mipLevels <= 0)
            mipLevels = 1;
        mMips.resize(mipLevels);

        // mips filter
        ImageParams mipParams;
        mipParams.s = GL_CLAMP_TO_EDGE;
        mipParams.r = GL_CLAMP_TO_EDGE;
        mipParams.t = GL_CLAMP_TO_EDGE;
        mipParams.minFilter = GL_LINEAR;
        mipParams.magFilter = GL_LINEAR;

        // mips data
        Image mipData;
        mipData.internalFormat = GL_RGB16F;
        mipData.pixelFormat = GL_RGB;
        mipData.pixelType = PixelType::FLOAT;

        glm::ivec2 mipSize = mResolution / 2;

        // init mips
        for (ColorAttachment& mip : mMips) {
            mip.image = mipData;
            mip.params = mipParams;
            mip.image.width = mipSize.x;
            mip.image.height = mipSize.y;
            mip.init();

            mipSize /= 2;
        }
    }

    void BloomRenderer::resize(int w, int h) {
        mResolution = { w, h };

        mFrame.bind();

        mMixColor.free();
        for (auto& mip : mMips) {
            mip.free();
        }

        initMixColor();
        initMips();

        mFrame.colors = {mMips[0] };
        mFrame.attachColors();
        mFrame.complete();
    }

    void BloomRenderer::render() {
        mFrame.bind();
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);
        renderDownsample();
        renderUpsample();
        renderMix();
    }

    void BloomRenderer::renderDownsample() {
        mDownsampleShader.use();

        ImageBuffer& hdrBuffer = getHdrBuffer();
        hdrBuffer.activate(0);
        hdrBuffer.bind();

        mDownsampleShader.resolution.value = mResolution;
        mDownsampleShader.setUniform(mDownsampleShader.resolution);

        auto& mip_level = mDownsampleShader.mipLevel;
        mip_level.value = 0;
        mDownsampleShader.setUniform(mip_level);

        for (int i = 0 ; i < mipLevels ; i++) {
            auto& mip = mMips[i];
            glViewport(0, 0, mip.image.width, mip.image.height);
            mip.attach();
            mDrawable.draw();

            mDownsampleShader.resolution.value = { mip.image.width, mip.image.height };
            mDownsampleShader.setUniform(mDownsampleShader.resolution);

            mip.buffer.bind();

            if (i == 0) {
                mip_level.value = 1;
                mDownsampleShader.setUniform(mip_level);
            }
        }
    }

    void BloomRenderer::renderUpsample() {
        // Enable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        mUpsampleShader.use();
        mUpsampleShader.update();

        for (int i = mipLevels - 1 ; i > 0 ; i--) {
            auto& mip = mMips[i];
            auto& nextMip = mMips[i - 1];

            mip.buffer.bind();

            glViewport(0, 0, nextMip.image.width, nextMip.image.height);
            nextMip.attach();

            mDrawable.draw();
        }

        glDisable(GL_BLEND);
    }

    void BloomRenderer::renderMix() {
        mMixColor.attach();
        glViewport(0, 0, mResolution.x, mResolution.y);
        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mMixShader.use();
        mMixShader.update();

        mDrawable.draw();
    }

}