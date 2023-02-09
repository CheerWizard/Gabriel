#include <core.h>

#include <window.h>
#include <geometry.h>
#include <draw.h>

namespace app {

    static bool s_running = true;

    static void window_close() {
        print("window_close()");
    }

    static void window_resized(int w, int h) {
        print("window_resized()");
    }

    static void framebuffer_resized(int w, int h) {
        gl::set_view_port(0, 0, w, h);
    }

    static void key_press(int key) {
        print("key_press()");
        if (key == GLFW_KEY_ESCAPE) {
            win::close();
        }
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

    static u32 triangle_shader_program;
    static u32 triangle_vao;
    static u32 triangle_vbo;

    static u32 rect_shader_program;
    static u32 rect_vao;
    static u32 rect_vbo;
    static u32 rect_ibo;

    static std::vector<gl::triangle> triangles;

    static gl::triangle triangle_init(u32 i) {
        gl::triangle triangle;
        float offset = i + 1;

        triangle.v0.pos.x *= offset;
        triangle.v0.pos.y *= offset;
        triangle.v0.pos.z *= offset;

        triangle.v1.pos.x *= offset;
        triangle.v1.pos.y *= offset;
        triangle.v1.pos.z *= offset;

        triangle.v2.pos.x *= offset;
        triangle.v2.pos.y *= offset;
        triangle.v2.pos.z *= offset;

        return triangle;
    }

    static void init() {
        win::init({ 800, 600, "CGP" });

        win::event_registry::window_close = window_close;
        win::event_registry::window_resized = window_resized;
        win::event_registry::framebuffer_resized = framebuffer_resized;
        win::event_registry::key_press = key_press;
        win::event_registry::key_release = key_release;
        win::event_registry::mouse_press = mouse_press;
        win::event_registry::mouse_release = mouse_release;

        win::event_registry_update();

        triangles = gl::triangles_init(triangle_shader_program, triangle_vao, triangle_vbo, 2, triangle_init);

        gl::rect_init(rect_shader_program, rect_vao, rect_vbo, rect_ibo);

        gl::polygon_line();
    }

    static void free() {
        gl::triangle_free(triangle_shader_program, triangle_vao, triangle_vbo);
        gl::rect_free(rect_shader_program, rect_vao, rect_vbo, rect_ibo);
        win::free();
    }

    void run() {
        init();
        while (s_running) {
            s_running = win::is_open();
            win::update();
            gl::clear_display(COLOR_CLEAR);
//            gl::draw_triangle(triangle_shader_program, triangle_vao);
//            gl::draw_rect(rect_shader_program, rect_vao, rect_ibo);
            gl::draw_triangles(triangle_shader_program, triangle_vao, triangles.size());
        }
        free();
    }

}