#include <ssao.h>
#include <random>
#include <math_functions.h>
#include <commands.h>

namespace gl {

    void SsaoShader::init(int width, int height) {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/ssao.frag");
        complete();
        params.resolution.value = { width, height };
        params.init();
    }

    void SsaoShader::update() {
        bind_sampler(params.positions_sampler, params.positions);
        bind_sampler(params.normals_sampler, params.normals);
        bind_sampler(params.noise_sampler, params.noise);

        set_uniform(params.resolution);
        set_uniform(params.noise_size);

        set_uniform_array(params.samples);

        params.samples_size.value = (int) params.samples.size();
        set_uniform(params.samples_size);
        set_uniform(params.sample_radius);
        set_uniform(params.sample_bias);

        set_uniform(params.occlusion_power);
    }

    void SsaoShader::resize(int width, int height) {
        params.resolution.value = { width, height };
    }

    void SsaoBlurShader::init() {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/ssao_blur.frag");
        complete();
    }

    void SsaoBlurShader::update(const ImageBuffer &ssao_image) {
        ssao_image.activate(0);
        ssao_image.bind();
    }

    SsaoRenderer::SsaoRenderer(int width, int height) {
        ssao_shader.init(width, height);
        blur_shader.init();

        drawable.init();

        // SSAO frame
        ColorAttachment color = { 0, width, height };
        color.image.internal_format = GL_RED;
        color.image.pixel_format = GL_RED;
        color.image.pixel_type = PixelType::FLOAT;
        color.params.min_filter = GL_NEAREST;
        color.params.mag_filter = GL_NEAREST;
        color.params.s = GL_REPEAT;
        color.params.t = GL_REPEAT;
        color.params.r = GL_REPEAT;
        fbo.colors = { color };
        fbo.init();
        fbo.init_colors();
        fbo.attach_colors();
        fbo.complete();
        // Blur frame
        ColorAttachment blur_color = { 0, width, height };
        blur_color.image.internal_format = GL_RED;
        blur_color.image.pixel_format = GL_RED;
        blur_color.image.pixel_type = PixelType::FLOAT;
        blur_color.params.min_filter = GL_NEAREST;
        blur_color.params.mag_filter = GL_NEAREST;
        blur_color.params.s = GL_REPEAT;
        blur_color.params.t = GL_REPEAT;
        blur_color.params.r = GL_REPEAT;
        blur_fbo.colors = { blur_color };
        blur_fbo.init();
        blur_fbo.init_colors();
        blur_fbo.attach_colors();
        blur_fbo.complete();

        ssao_image = fbo.colors[0].buffer;
        render_target = blur_fbo.colors[0].buffer;
    }

    SsaoRenderer::~SsaoRenderer() {
        fbo.free();
        ssao_shader.free();
        ssao_shader.params.free();
        blur_fbo.free();
        blur_shader.free();
        drawable.free();
    }

    void SsaoRenderer::render() {
        // Occlusion part
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        ssao_shader.use();
        ssao_shader.update();

        drawable.draw();

        // Blur part
        blur_fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        blur_shader.use();
        blur_shader.update(ssao_image);

        drawable.draw();
    }

    void SsaoRenderer::resize(int w, int h) {
        fbo.resize(w, h);
        blur_fbo.resize(w, h);
        ssao_shader.resize(w, h);
    }

    void SsaoParams::init() {
        // setup kernels
        std::uniform_real_distribution<float> random_floats(0.0, 1.0);
        std::default_random_engine generator;
        int samples_size = (int) samples.size();
        for (u32 i = 0; i < samples_size; i++) {
            glm::vec3 sample = {
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator)
            };
            sample = glm::normalize(sample);
            sample *= random_floats(generator);
            float scale = (float) i / samples_size;
            scale = gl::lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            samples[i] = sample;
        }
        // setup noise
        std::vector<glm::vec3> noises(noise_size.value * noise_size.value);
        for (u32 i = 0; i < noise_size.value * noise_size.value; i++) {
            noises[i] = {
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    0
            };
        }
        // setup noise texture
        ImageParams noise_params;
        noise_params.min_filter = GL_NEAREST;
        noise_params.mag_filter = GL_NEAREST;
        noise_params.s = GL_REPEAT;
        noise_params.t = GL_REPEAT;
        noise_params.r = GL_REPEAT;
        noise.init();
        Image noise_image = { noise_size.value, noise_size.value, 3, GL_RGBA32F, GL_RGB, PixelType::FLOAT, &noises[0] };
        noise.load(noise_image, noise_params);
    }

    void SsaoParams::free() {
        noise.free();
    }

}