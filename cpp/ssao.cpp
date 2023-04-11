#include <ssao.h>
#include <random>
#include <math_functions.h>
#include <commands.h>

namespace gl {

    Shader SSAO::shader;
    Shader SSAO::blur_shader;
    FrameBuffer SSAO::fbo;
    FrameBuffer SSAO::blur_fbo;
    VertexArray SSAO::vao;
    ImageBuffer SSAO::render_target;

    void SSAO::init(int width, int height) {
        // SSAO quad
        vao.init();
        // SSAO shaders
        shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/ssao.frag"
        );
        blur_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/ssao_blur.frag"
        );
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
        // SSAO Blur frame
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

        render_target = blur_fbo.colors[0].buffer;
    }

    void SSAO::free() {
        vao.free();
        shader.free();
        blur_shader.free();
        fbo.free();
        blur_fbo.free();
    }

    void SSAO::render(SSAO_Pass &pass) {
        // Occlusion part
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.bind_sampler("v_positions", 0, pass.positions);
        shader.bind_sampler("v_normals", 1, pass.normals);
        shader.bind_sampler("noise", 2, pass.noise);

        shader.set_uniform_args("resolution", pass.resolution);
        shader.set_uniform_args("noise_size", pass.noise_size);
        int samples_size = (int) pass.samples.size();
        shader.set_uniform_args("samples_size", samples_size);
        shader.set_uniform_args("sample_radius", pass.sample_radius);
        shader.set_uniform_args("sample_bias", pass.sample_bias);
        shader.set_uniform_args("occlusion_power", pass.occlusion_power);

        UniformArrayV3F samples = { "samples", pass.samples };
        shader.set_uniform_array(samples);

        vao.draw_quad();

        // Blur part
        blur_fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        blur_shader.use();

        blur_shader.bind_sampler({ "ssao", 0 }, fbo.colors[0].buffer);

        vao.draw_quad();
    }

    void SSAO::resize(int w, int h) {
        fbo.resize(w, h);
        blur_fbo.resize(w, h);
    }

    void SSAO_Pass::init() {
        // SSAO kernels
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
        // SSAO noise
        std::vector<glm::vec3> noises(noise_size * noise_size);
        for (u32 i = 0; i < noise_size * noise_size; i++) {
            noises[i] = {
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    0
            };
        }
        // SSAO noise texture
        ImageParams noise_params;
        noise_params.min_filter = GL_NEAREST;
        noise_params.mag_filter = GL_NEAREST;
        noise_params.s = GL_REPEAT;
        noise_params.t = GL_REPEAT;
        noise_params.r = GL_REPEAT;
        noise.init();
        Image noise_image = { noise_size, noise_size, 3, GL_RGBA32F, GL_RGB, PixelType::FLOAT, &noises[0] };
        noise.load(noise_image, noise_params);
    }

    void SSAO_Pass::free() {
        noise.free();
    }

}