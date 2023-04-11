#include <bloom.h>

namespace gl {

    void Bloom::init() {
        // setup shaders
        downsample_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/bloom/bloom_downsample.frag"
        );
        upsample_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/bloom/bloom_upsample.frag"
        );
        mix_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/bloom/bloom_mix.frag"
        );
        // setup screen vao
        vao.init();
        // setup Bloom frame
        init_mix_color();
        init_mips();
        fbo.colors = { mips[0] };
        fbo.init();
        fbo.attach_colors();
        fbo.complete();

        set_resolution(resolution);
        set_filter_radius(filter_radius);
        set_bloom_strength(bloom_strength);
    }

    void Bloom::free() {
        fbo.free();
        vao.free();
        downsample_shader.free();
        upsample_shader.free();
        mix_shader.free();
        mix_color.free();
        for (auto& mip : mips) {
            mip.free();
        }
    }

    void Bloom::init_mix_color() {
        // filter
        mix_color.params.s = GL_CLAMP_TO_EDGE;
        mix_color.params.r = GL_CLAMP_TO_EDGE;
        mix_color.params.t = GL_CLAMP_TO_EDGE;
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

    void Bloom::init_mips() {
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

    void Bloom::set_resolution(const glm::ivec2& resolution) {
        this->resolution = resolution;
        downsample_shader.use();
        downsample_shader.set_uniform_args("resolution", this->resolution);
    }

    void Bloom::set_filter_radius(float filter_radius) {
        this->filter_radius = filter_radius;
        upsample_shader.use();
        upsample_shader.set_uniform_args("filter_radius", filter_radius);
    }

    void Bloom::set_bloom_strength(float bloom_strength) {
        this->bloom_strength = bloom_strength;
        mix_shader.use();
        mix_shader.set_uniform_args("bloom_strength", bloom_strength);
    }

    void Bloom::resize(int w, int h) {
        set_resolution({ w, h });

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

    void Bloom::render() {
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);
        render_downsample();
        render_upsample();
        render_mix();
        render_target = mix_color.buffer;
    }

    void Bloom::render_downsample() {
        downsample_shader.use();

        src.activate(0);
        src.bind();

        downsample_shader.set_uniform_args("resolution", resolution);

        int mip_level = 0;
        downsample_shader.set_uniform_args("mip_level", mip_level);

        for (int i = 0 ; i < mip_levels ; i++) {
            auto& mip = mips[i];
            glViewport(0, 0, mip.image.width, mip.image.height);
            mip.attach();
            vao.draw_quad();

            glm::ivec2 mip_resolution = { mip.image.width, mip.image.height };
            downsample_shader.set_uniform_args("resolution", mip_resolution);
            mip.buffer.bind();

            if (i == 0) {
                mip_level = 1;
                downsample_shader.set_uniform_args("mip_level", mip_level);
            }
        }
    }

    void Bloom::render_upsample() {
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

            vao.draw_quad();
        }

        glDisable(GL_BLEND);
    }

    void Bloom::render_mix() {
        mix_color.attach();
        glViewport(0, 0, resolution.x, resolution.y);
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        mix_shader.use();

        mix_shader.bind_sampler("hdr", 0, src);
        mix_shader.bind_sampler("bloom", 1, fbo.colors[0].buffer);

        vao.draw_quad();
    }

}