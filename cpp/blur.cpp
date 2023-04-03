#include <blur.h>
#include <commands.h>
#include <screen.h>

namespace gl {

    Shader Blur::shader;
    FrameBuffer Blur::fbo;
    VertexArray Blur::vao;
    float Blur::offset = 1.0 / 300.0;
    Texture Blur::src;
    Texture Blur::render_target;

    void Blur::init(int w, int h) {
        shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/blur.frag"
        );

        fbo.init();
        ColorAttachment color = { 0, w, h };
        color.data.internal_format = GL_RGB;
        color.data.data_format = GL_RGB;
        color.data.primitive_type = GL_UNSIGNED_BYTE;
        fbo.colors = { color };
        fbo.init_colors();
        fbo.attach_colors();
        fbo.complete();

        shader.use();
        shader.set_uniform_args("offset", offset);

        vao.init();
    }

    void Blur::free() {
        shader.free();
        fbo.free();
        vao.free();
    }

    void Blur::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void Blur::render() {
        fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();

        src.activate(0);
        src.bind();

        vao.draw_quad();

        render_target = fbo.colors[0].view;
    }

}