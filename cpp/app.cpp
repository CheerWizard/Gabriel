#include <app.h>

#include <window.h>
#include <draw.h>
#include <texture.h>
#include <camera.h>
#include <light.h>
#include <material.h>

namespace app {

    static bool s_running = true;
    static float s_dt = 6;

    static u32 cube_shader;
    static u32 cube_vao;
    static u32 cube_vbo;
    static u32 cube_ibo;

    static gl::transform s_transform = {
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };

    static gl::camera s_camera;

    static gl::light s_light;

    static gl::material s_material;

    static void window_close() {
        print("window_close()");
    }

    static void window_resized(int w, int h) {
        print("window_resized()");
    }

    static void framebuffer_resized(int w, int h) {
        glViewport(0, 0, w, h);
        gl::camera_resize(cube_shader, s_camera, w, h);
    }

    static void key_press(int key) {
        print("key_press()");
        if (key == KEY::Escape)
            win::close();
    }

    static void key_release(int key) {
        print("key_release()");
    }

    static void mouse_press(int button) {
        print("mouse_press()");
    }

    static void mouse_release(int button) {
        print("mouse_release()");
    }

    static void mouse_cursor(double x, double y) {
        print("mouse_cursor");
        gl::camera_look(s_camera, x, y);
    }

    static void mouse_scroll(double x, double y) {
        print("mouse_scroll()");
        gl::camera_zoom(cube_shader, s_camera, x, y);
    }

    static void init() {
        win::init({ 800, 600, "CGP", true });
//        win::disable_cursor();

        win::event_registry::window_close = window_close;
        win::event_registry::window_resized = window_resized;
        win::event_registry::framebuffer_resized = framebuffer_resized;
        win::event_registry::key_press = key_press;
        win::event_registry::key_release = key_release;
        win::event_registry::mouse_press = mouse_press;
        win::event_registry::mouse_release = mouse_release;
        win::event_registry::mouse_cursor = mouse_cursor;
        win::event_registry::mouse_scroll = mouse_scroll;

        win::event_registry_update();

        gl::cube_uv_normal_init(cube_shader, cube_vao, cube_vbo, cube_ibo);

        s_camera = gl::camera_init(cube_shader);

        s_material = gl::material_init(
                "images/container.png",
                "images/container_specular.png",
                "images/container_emission.jpg"
        );

        s_light.color = { 1, 1, 1, 1 };
        gl::light_init(cube_shader, s_camera, s_light);

        gl::shader_use(cube_shader);
        gl::transform_update(cube_shader, s_transform);
        gl::material_update(cube_shader, s_material);
        gl::light_update(cube_shader, s_camera, s_light);

        gl::polygon_fill();
        glEnable(GL_DEPTH_TEST);
    }

    static void free() {
        gl::free(cube_shader, cube_vao, cube_vbo, cube_ibo);
        gl::light_free();
        win::free();
    }

    void run() {
        init();

        while (s_running) {
            float t = glfwGetTime();
            s_running = win::is_open();
            win::update();
            gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float camera_speed = s_camera.speed / s_dt;
            glm::vec3& camera_pos = s_camera.position;

            if (win::is_key_press(KEY::W)) {
                camera_pos += camera_speed * s_camera.front;
            }
            else if (win::is_key_press(KEY::A)) {
                camera_pos -= glm::normalize(glm::cross(s_camera.front, s_camera.up)) * camera_speed;
            }
            else if (win::is_key_press(KEY::S)) {
                camera_pos -= camera_speed * s_camera.front;
            }
            else if (win::is_key_press(KEY::D)) {
                camera_pos += glm::normalize(glm::cross(s_camera.front, s_camera.up)) * camera_speed;
            }

            gl::light_update(cube_shader, s_camera, s_light);

            float dt = s_dt;
            float f = 1 / pow(log(dt), 2);
            s_transform.rotation.y += f;

            glm::vec3 lightColor;
            lightColor.x = sin(t * 2.0f);
            lightColor.y = sin(t * 0.7f);
            lightColor.z = sin(t * 1.3f);
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

            s_light.ambient = ambientColor;
            s_light.diffuse = diffuseColor;

            gl::shader_use(cube_shader);
            gl::camera_view_update(cube_shader, s_camera);
            gl::transform_update(cube_shader, s_transform);
            gl::material_update(cube_shader, s_material);
            gl::shader_set_uniformf(cube_shader, "time", t);
            gl::shader_set_uniform3v(cube_shader, "emission_color", lightColor);
            gl::draw_cube(cube_vao, cube_ibo);

            s_dt = ((float)glfwGetTime() - t) * 1000;
        }

        free();
    }

}