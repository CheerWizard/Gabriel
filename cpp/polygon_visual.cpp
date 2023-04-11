#include <polygon_visual.h>

namespace gl {

    void PolygonVisualRenderer::init() {
        shader.init(
                "shaders/polygon_visual.vert",
                "shaders/polygon_visual.frag"
        );
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