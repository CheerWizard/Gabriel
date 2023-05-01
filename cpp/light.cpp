#include <light.h>

namespace gl {

    static Shader light_present_shader;
    static DrawableElements light_present_drawable;

    void LightPresent::init() {
        light_present_shader.add_vertex_stage("shaders/light_present.vert");
        light_present_shader.add_fragment_stage("shaders/light_present.frag");
        light_present_shader.complete();

        presentation.init(light_present_drawable);
    }

    void LightPresent::free() {
        light_present_shader.free();
        light_present_drawable.free();
    }

    void LightPresent::update() {
        light_present_shader.use();
        light_present_shader.set_uniform_args("object_id", id);
        light_present_shader.set_uniform_args("draw_id", id);
        light_present_shader.set_uniform_args("color", color);
        transform.update(light_present_shader);
        light_present_drawable.draw();
    }

}