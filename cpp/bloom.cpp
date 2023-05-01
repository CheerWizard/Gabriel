#include <bloom.h>

namespace gl {

    void BloomUpsampleShader::init() {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/bloom/bloom_upsample.frag");
        complete();
        update_filter_radius();
    }

    void BloomUpsampleShader::update_filter_radius() {
        use();
        set_uniform(filter_radius);
    }

    void BloomDownsampleShader::init(const glm::ivec2& resolution) {
        this->resolution.value = resolution;
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/bloom/bloom_downsample.frag");
        complete();
        update_resolution();
    }

    void BloomDownsampleShader::update_resolution() {
        use();
        set_uniform(resolution);
    }

    void BloomMixShader::init() {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/bloom/bloom_mix.frag");
        complete();
        update_bloom_strength();
    }

    void BloomMixShader::update_bloom_strength() {
        use();
        set_uniform(bloom_strength);
    }

    void BloomMixShader::update() {
        bind_sampler(hdr_sampler, hdr_buffer);
        bind_sampler(bloom_sampler, bloom_buffer);
    }

    BloomRenderer::BloomRenderer(int width, int height) {
        resolution = { width, height };
        downsample_shader.init(resolution);
        upsample_shader.init();
        mix_shader.init();

        drawable.init();

        init_mix_color();
        init_mips();
        fbo.colors = { mips[0] };
        fbo.init();
        fbo.attach_colors();
        fbo.complete();

        mix_shader.bloom_buffer = fbo.colors[0].buffer;
        render_target = mix_color.buffer;
    }

    BloomRenderer::~BloomRenderer() {
        fbo.free();
        downsample_shader.free();
        upsample_shader.free();
        mix_shader.free();
        drawable.free();

        mix_color.free();
        for (auto& mip : mips) {
            mip.free();
        }
    }

    void BloomRenderer::init_mix_color() {
        // filter
        mix_color.params.s = GL_CLAMP_TO_EDGE;
        mix_color.params.t = GL_CLAMP_TO_EDGE;
        mix_color.params.r = GL_CLAMP_TO_EDGE;
        mix_color.params.min_filter = GL_LINEAR;
        mix_color.params.mag_filter = GL_LINEAR;
        // data
        mix_color.image.internal_format = GL_RGB16F;
        mix_color.image.pixel_format = GL_RGB;
        mix_color.image.pixel_type = PixelType::FLOAT;
        mix_color.image.width = resolution.x;
        mix_color.image.height = resolution.y;

        mix_color.init();
    }

    void BloomRenderer::init_mips() {
        if (mip_levels <= 0)
            mip_levels = 1;
        mips.resize(mip_levels);

        // mips filter
        ImageParams mip_params;
        mip_params.s = GL_CLAMP_TO_EDGE;
        mip_params.r = GL_CLAMP_TO_EDGE;
        mip_params.t = GL_CLAMP_TO_EDGE;
        mip_params.min_filter = GL_LINEAR;
        mip_params.mag_filter = GL_LINEAR;

        // mips data
        Image mip_data;
        mip_data.internal_format = GL_RGB16F;
        mip_data.pixel_format = GL_RGB;
        mip_data.pixel_type = PixelType::FLOAT;

        glm::ivec2 mip_size = resolution / 2;

        // init mips
        for (ColorAttachment& mip : mips) {
            mip.image = mip_data;
            mip.params = mip_params;
            mip.image.width = mip_size.x;
            mip.image.height = mip_size.y;
            mip.init();

            mip_size /= 2;
        }
    }

    void BloomRenderer::resize(int w, int h) {
        resolution = { w, h };

        fbo.bind();

        mix_color.free();
        for (auto& mip : mips) {
            mip.free();
        }

        init_mix_color();
        init_mips();

        fbo.colors = { mips[0] };
        fbo.attach_colors();
        fbo.complete();
    }

    void BloomRenderer::render() {
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);
        render_downsample();
        render_upsample();
        render_mix();
    }

    void BloomRenderer::render_downsample() {
        downsample_shader.use();

        ImageBuffer& hdr_buffer = get_hdr_buffer();
        hdr_buffer.activate(0);
        hdr_buffer.bind();

        downsample_shader.resolution.value = resolution;
        downsample_shader.set_uniform(downsample_shader.resolution);

        auto& mip_level = downsample_shader.mip_level;
        mip_level.value = 0;
        downsample_shader.set_uniform(mip_level);

        for (int i = 0 ; i < mip_levels ; i++) {
            auto& mip = mips[i];
            glViewport(0, 0, mip.image.width, mip.image.height);
            mip.attach();
            drawable.draw();

            downsample_shader.resolution.value = { mip.image.width, mip.image.height };
            downsample_shader.set_uniform(downsample_shader.resolution);

            mip.buffer.bind();

            if (i == 0) {
                mip_level.value = 1;
                downsample_shader.set_uniform(mip_level);
            }
        }
    }

    void BloomRenderer::render_upsample() {
        // Enable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        upsample_shader.use();

        for (int i = mip_levels - 1 ; i > 0 ; i--) {
            auto& mip = mips[i];
            auto& next_mip = mips[i - 1];

            mip.buffer.bind();

            glViewport(0, 0, next_mip.image.width, next_mip.image.height);
            next_mip.attach();

            drawable.draw();
        }

        glDisable(GL_BLEND);
    }

    void BloomRenderer::render_mix() {
        mix_color.attach();
        glViewport(0, 0, resolution.x, resolution.y);
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mix_shader.use();
        mix_shader.update();

        drawable.draw();
    }

}