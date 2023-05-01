#include <hdr.h>
#include <commands.h>

namespace gl {

    void HdrShader::init() {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/hdr.frag");
        complete();
        update_exposure();
        update_shiny_strength();
    }

    void HdrShader::update() {
        bind_sampler(params.scene_sampler, params.scene_buffer);
        bind_sampler(params.shiny_sampler, params.shiny_buffer);
    }

    void HdrShader::update_exposure() {
        use();
        set_uniform(params.exposure);
    }

    void HdrShader::update_shiny_strength() {
        use();
        set_uniform(params.shiny_strength);
    }

    HdrRenderer::HdrRenderer(int w, int h) {
        shader.init();
        drawable.init();

        ColorAttachment color;
        color.image.internal_format = GL_RGB;
        color.image.pixel_format = GL_RGB;
        color.image.pixel_type = PixelType::U8;
        color.image.width = w;
        color.image.height = h;
        color.init();

        fbo.init();
        fbo.colors = { color };
        fbo.attach_colors();
        fbo.complete();

        render_target = fbo.colors[0].buffer;
    }

    HdrRenderer::~HdrRenderer() {
        drawable.free();
        shader.free();
        fbo.free();
        shader.params.shiny_buffer.free();
    }

    void HdrRenderer::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void HdrRenderer::render() {
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.update();

        drawable.draw();
    }

    void HdrRenderer::update_exposure() {
        shader.update_exposure();
    }

    void HdrRenderer::update_shiny_strength() {
        shader.update_shiny_strength();
    }

}