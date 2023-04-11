#include <outline.h>

namespace gl {

    void OutlineRenderer::init() {
        shader.init(
            "shaders/outline.vert",
            "shaders/outline.frag"
        );
    }

    void OutlineRenderer::free() {
        shader.free();
    }

    void OutlineRenderer::begin() {
        glStencilMask(GL_FALSE);
        glStencilFunc(GL_NOTEQUAL, 1, GL_TRUE);
        glDisable(GL_DEPTH_TEST);
        shader.use();
    }

    void OutlineRenderer::end() {
        glStencilMask(GL_TRUE);
        glStencilFunc(GL_ALWAYS, 1, GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    void OutlineRenderer::render(Outline& outline, Transform& transform, DrawableElements& drawable) {
        shader.set_uniform_args<float>("outline_thickness", outline.thickness);
        shader.set_uniform_args<glm::vec4>("outline_color", outline.color);
        transform.update(shader);
        drawable.draw();
    }

}