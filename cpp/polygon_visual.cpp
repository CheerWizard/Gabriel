#include <polygon_visual.h>

namespace gl {

    void PolygonVisualRenderer::init() {
        shader.add_vertex_stage("shaders/polygon_visual.vert");
        shader.add_fragment_stage("shaders/polygon_visual.frag");
        shader.complete();
    }

    void PolygonVisualRenderer::free() {
        shader.free();
    }

    void PolygonVisualRenderer::begin() {
        gl::polygon_line();
        shader.use();
    }

    void PolygonVisualRenderer::end() {
        gl::polygon_fill();
    }

    void PolygonVisualRenderer::render(PolygonVisual& polygon_visual, Transform& transform, DrawableElements& drawable) {
        shader.set_uniform_args("color", polygon_visual.color);
        shader.set_uniform_args("thickness", polygon_visual.thickness);
        transform.update(shader);
        drawable.draw();
    }

}