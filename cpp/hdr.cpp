#include <hdr.h>
#include <commands.h>

namespace gl {

    void HDR_Renderer::init(int w, int h) {
        vao.init();
        shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/hdr.frag"
        );
        fbo.init();
        ColorAttachment color;
        color.image.internal_format = GL_RGB;
        color.image.pixel_format = GL_RGB;
        color.image.pixel_type = PixelType::U8;
        color.image.width = w;
        color.image.height = h;
        fbo.colors = { color };
        fbo.init_colors();
        fbo.attach_colors();
        fbo.complete();

        set_exposure(exposure);
        set_shiny_strength(shiny_strength);
    }

    void HDR_Renderer::free() {
        vao.free();
        shader.free();
        fbo.free();
        shiny.free();
    }

    void HDR_Renderer::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void HDR_Renderer::render() {
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.bind_sampler("scene", 0, src);
        shader.bind_sampler("shiny", 1, shiny);

        vao.draw_quad();

        render_target = fbo.colors[0].buffer;
    }

    void HDR_Renderer::set_exposure(float exposure) {
        this->exposure = exposure;
        shader.use();
        shader.set_uniform_args("exposure", exposure);
    }

    void HDR_Renderer::set_shiny_strength(float shiny_strength) {
        this->shiny_strength = shiny_strength;
        shader.use();
        shader.set_uniform_args("shiny_strength", shiny_strength);
    }

}