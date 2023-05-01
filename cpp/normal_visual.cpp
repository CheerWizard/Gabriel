#include <normal_visual.h>

namespace gl {

    void NormalVisualRenderer::init() {
        shader.add_vertex_stage("shaders/normal_visual.vert");
        shader.add_fragment_stage("shaders/normal_visual.frag");
        shader.add_geometry_stage("shaders/normal_visual.geom");
        shader.complete();
    }

    void NormalVisualRenderer::free() {
        shader.free();
    }

    void NormalVisualRenderer::render(NormalVisual& normal_visual, Transform& transform, DrawableElements& drawable) {
        shader.set_uniform_args("color", normal_visual.color);
        shader.set_uniform_args("length", normal_visual.length);
        transform.update(shader);
        drawable.draw();
    }

    void NormalVisualRenderer::begin() {
        shader.use();
    }

}