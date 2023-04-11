#include <screen.h>
#include <frame.h>
#include <commands.h>

namespace gl {

    Shader Screen::shader;
    VertexArray Screen::vao;
    ImageBuffer Screen::src;
    float Screen::gamma = 2.2f; // average value on most monitors

    void Screen::init() {
        shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/screen.frag"
        );
        vao.init();
        set_gamma(gamma);
    }

    void Screen::free() {
        shader.free();
        vao.free();
        src.free();
    }

    void Screen::render() {
        FrameBuffer::unbind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();

        src.activate(0);
        src.bind();

        vao.draw_quad();
    }

    void Screen::set_gamma(float gamma) {
        Screen::gamma = gamma;
        shader.use();
        shader.set_uniform_args("gamma", gamma);
    }

}