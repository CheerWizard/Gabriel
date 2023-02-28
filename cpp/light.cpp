#include <light.h>
#include <draw.h>

namespace gl {

    static u32 light_present_shader = 0;
    static gl::drawable_elements light_present_drawable;
    static gl::light_present* s_present;

    void light_present_init(light_present* present) {
        light_present_shader = shader_init({
            "shaders/light_present.vert",
            "shaders/light_present.frag"
        });

        present->presentation = cube_solid_init(
                light_present_drawable,
                present->color
        );

        s_present = present;
    }

    void light_present_free() {
        shader_free(light_present_shader);
        drawable_free(light_present_drawable);
    }

    void light_present_update() {
        shader_use(light_present_shader);
        transform_update(light_present_shader, s_present->transform);
        draw(light_present_drawable);
    }

}