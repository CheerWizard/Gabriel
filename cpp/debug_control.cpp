#include <debug_control.h>

namespace gl {

    void DebugControlPipeline::init(int w, int h) {
        init_frame(w, h);
        polygon_visual_renderer.init();
        normal_visual_renderer.init();
    }

    void DebugControlPipeline::init_frame(int w, int h) {
        ColorAttachment color = { 0, w, h };
        color.image.internal_format = GL_RGB;
        color.image.pixel_format = GL_RGB;
        color.image.pixel_type = PixelType::U8;
        color.params.min_filter = GL_LINEAR;
        color.params.mag_filter= GL_LINEAR;
        color.init();

        fbo.rbo = { w, h };
        fbo.rbo.init();

        fbo.init();

        fbo.colors = { color };
        fbo.attach_colors();
        fbo.attach_render_buffer();

        fbo.complete();

        render_target = fbo.colors[0].buffer;
    }

    void DebugControlPipeline::free() {
        fbo.free();
        polygon_visual_renderer.free();
        normal_visual_renderer.free();
    }

    void DebugControlPipeline::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void DebugControlPipeline::render() {
        fbo.bind();
        glEnable(GL_DEPTH_TEST);

        // render visual polygons
        polygon_visual_renderer.begin();
        scene->each_component<PolygonVisual>([this](PolygonVisual* polygon_visual) {
            ecs::EntityID entity_id = polygon_visual->entity_id;
            polygon_visual_renderer.render(
                    *polygon_visual,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id)
            );
        });
        polygon_visual_renderer.end();

        // render visual normals
        normal_visual_renderer.begin();
        scene->each_component<NormalVisual>([this](NormalVisual* normal_visual) {
            ecs::EntityID entity_id = normal_visual->entity_id;
            normal_visual_renderer.render(
                    *normal_visual,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id)
            );
        });

        glDisable(GL_DEPTH_TEST);
    }

}