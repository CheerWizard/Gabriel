#include <outline.h>

namespace gl {

    static Shader outline_shader;

    void Outline::init() {
        outline_shader.init(
            "shaders/outline.vert",
            "shaders/outline.frag"
        );
        outline_shader.use();
        outline_shader.set_uniform_args<float>("outline_thickness", thickness);
        outline_shader.set_uniform_args<glm::vec4>("outline_color", color);
    }

    void Outline::free() {
        outline_shader.free();
    }

    void Outline::begin() {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
    }

    void Outline::end() {
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    void Outline::draw(Transform& transform, const DrawableElements& drawable) {
        outline_shader.use();
        transform.update(outline_shader);
        outline_shader.set_uniform_args<float>("outline_thickness", thickness);
        outline_shader.set_uniform_args<glm::vec4>("outline_color", color);
        drawable.draw();
    }

}