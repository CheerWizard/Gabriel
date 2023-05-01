#include <screen.h>
#include <frame.h>

namespace gl {

    void ScreenShader::init() {
        add_vertex_stage("shaders/fullscreen_quad.vert");
        add_fragment_stage("shaders/screen.frag");
        complete();
        update_gamma();
    }

    void ScreenShader::update() {
        bind_sampler(params.scene_sampler, params.scene_buffer);
        bind_sampler(params.ui_sampler, params.ui_buffer);
#ifdef DEBUG
        bind_sampler(params.debug_control_sampler, params.debug_control_buffer);
#endif
    }

    void ScreenShader::update_gamma() {
        use();
        set_uniform(params.gamma);
    }

    ScreenRenderer::ScreenRenderer() {
        shader.init();
        drawable.init();
    }

    ScreenRenderer::~ScreenRenderer() {
        shader.free();
        drawable.free();
    }

    void ScreenRenderer::render() {
        FrameBuffer::unbind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.update();

        drawable.draw();
    }

    void ScreenRenderer::update_gamma() {
        shader.update_gamma();
    }

}