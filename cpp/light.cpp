#include <light.h>
#include <draw.h>

namespace gl {

    struct light_present_t final {
        u32 shader;
        u32 vao;
        u32 vbo;
        u32 ibo;
    };

    static light_present_t light_present;

    static void light_update_target(u32 shader, light& light) {
        shader_use(shader);
        shader_set_uniform3v(shader, "light_position", light.transform.translation);
        shader_set_uniform_struct3v(shader, "light", "ambient", light.ambient);
        shader_set_uniform_struct3v(shader, "light", "diffuse", light.diffuse);
        shader_set_uniform_struct3v(shader, "light", "specular", light.specular);
    }

    static void light_update_present(camera& camera, const light& light) {
        shader_use(light_present.shader);
        camera_perspective_update(light_present.shader, camera);
        camera_view_update(light_present.shader, camera);
        transform_update(light_present.shader, light.transform);
        draw_cube(light_present.vao, light_present.ibo);
    }

    void light_init(u32 shader, camera& camera, light& light) {
        light.presentation = cube_solid_init(
                light_present.shader,
                light_present.vao,
                light_present.vbo,
                light_present.ibo,
                light.color
        );

        light_update_present(camera, light);

        light_update_target(shader, light);
    }

    void light_free() {
        free(light_present.shader, light_present.vao, light_present.vbo, light_present.ibo);
    }

    void light_update(u32 shader, camera& camera, light& light) {
        // draw presentation of light
        light_update_present(camera, light);
        // update light position in target shader
        light_update_target(shader, light);
    }

}