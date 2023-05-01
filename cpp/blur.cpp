#include <blur.h>
#include <commands.h>
#include <screen.h>

namespace gl {

    void BlurShader::init() {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/blur.frag");
        complete();
        update_offset();
    }

    void BlurShader::update() {
        params.scene_buffer.activate(0);
        params.scene_buffer.bind();
    }

    void BlurShader::update_offset() {
        use();
        set_uniform(params.offset);
    }

    BlurRenderer::BlurRenderer(int w, int h) {
        shader.init();

        fbo.init();
        ColorAttachment color = { 0, w, h };
        color.image.internal_format = GL_RGB;
        color.image.pixel_format = GL_RGB;
        color.image.pixel_type = PixelType::U8;
        fbo.colors = { color };
        fbo.init_colors();
        fbo.attach_colors();
        fbo.complete();

        drawable.init();

        render_target = fbo.colors[0].buffer;
    }

    BlurRenderer::~BlurRenderer() {
        fbo.free();
        shader.free();
        drawable.free();
    }

    void BlurRenderer::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void BlurRenderer::render() {
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.update();
        drawable.draw();
    }

    void BlurRenderer::update_offset() {
        shader.update_offset();
    }

}