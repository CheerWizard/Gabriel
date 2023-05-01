#include <ui.h>

namespace gl {

    void UI_Pipeline::init(int w, int h) {
        init_frame(w, h);
        text2d_renderer.init();
        text3d_renderer.init();
    }

    void UI_Pipeline::free() {
        fbo.free();
        text2d_renderer.free();
        text3d_renderer.free();
    }

    void UI_Pipeline::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void UI_Pipeline::init_frame(int w, int h) {
        ColorAttachment ui_color = { 0, w, h };
        ui_color.image.internal_format = GL_RGBA8;
        ui_color.image.pixel_format = GL_RGBA;
        ui_color.image.pixel_type = PixelType::U8;
        ui_color.params.min_filter = GL_LINEAR;
        ui_color.params.mag_filter= GL_LINEAR;
        ui_color.init();

        fbo.rbo = { w, h };
        fbo.rbo.init();

        fbo.init();

        fbo.colors = { ui_color };
        fbo.attach_colors();
        fbo.attach_render_buffer();

        fbo.complete();

        render_target = fbo.colors[0].buffer;
    }

    void UI_Pipeline::render() {
        fbo.bind();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        text2d_renderer.begin();
        scene->each_component<Text2d>([this](Text2d* text) {
            text2d_renderer.render(*text);
        });

        text3d_renderer.begin();
        scene->each_component<Text3d>([this](Text3d* text) {
            text3d_renderer.render(*text);
        });

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

}