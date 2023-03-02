#include <outline.h>

namespace gl {

    static u32 outline_shader = 0;

    outline_t outline_init() {
        outline_t outline;

        outline_shader = shader_init({
            "shaders/outline.vert",
            "shaders/outline.frag"
        });

        shader_use(outline_shader);
        shader_set_uniformf(outline_shader, "outline_thickness", outline.thickness);
        shader_set_uniform4v(outline_shader, "outline_color", outline.color);

        return outline;
    }

    void outline_free() {
        shader_free(outline_shader);
    }

    void outline_begin() {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
    }

    void outline_end() {
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    void outline_draw(gl::transform& transform, const gl::drawable_elements& drawable, outline_t& outline) {
        shader_use(outline_shader);
        transform_update(outline_shader, transform);
        shader_set_uniformf(outline_shader, "outline_thickness", outline.thickness);
        shader_set_uniform4v(outline_shader, "outline_color", outline.color);
        draw(drawable);
    }

}