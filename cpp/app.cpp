#include <app.h>

#include <window.h>
#include <geometry.h>
#include <draw.h>
#include <texture.h>
#include <transform.h>
#include <cmath>

namespace app {

    static bool s_running = true;
    static float s_dt = 6;

    static u32 triangle_shader;
    static u32 triangle_vao;
    static u32 triangle_vbo;

    static u32 rect_shader;
    static u32 rect_vao;
    static u32 rect_vbo;
    static u32 rect_ibo;

    static u32 wall_tbo;
    static u32 smile_tbo;

    static gl::uniform_i wall_sampler = { "sampler1", 0 };
    static gl::uniform_i smile_sampler = { "sampler2", 1 };
    static gl::uniform_f mix_factor = { "mix_factor", 0.2f };

    static std::vector<gl::triangle_solid> triangles;
    static std::vector<gl::rect_uv> rects;

    static gl::uniform_v4f s_rect_color = { "color", { 1, 0, 0, 0 } };
    static gl::transform s_rect_transform = {
            { 0, 0, -5 }
    };

    static gl::ortho_data s_ortho = { 0.0f, 800.0f,0.0f, 600.0f,0.1f, 100.0f };

    static gl::uniform_m4f s_perspective = { "perspective" };

    static gl::camera s_camera;

    static float last_x = 400;
    static float last_y = 300;
    static bool first_mouse = true;
    static bool look_around = false;

    static void window_close() {
        print("window_close()");
    }

    static void window_resized(int w, int h) {
        print("window_resized()");
    }

    static void framebuffer_resized(int w, int h) {
        glViewport(0, 0, w, h);
        gl::shader_use(rect_shader);
        s_perspective.value = gl::mat_perspective({ 45, (float) w / (float) h });
        gl::shader_set_uniform(rect_shader, s_perspective);
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
        look_around = button == GLFW_MOUSE_BUTTON_LEFT;
    }

    static void mouse_release(int button) {
        print("mouse_release()");
        look_around = button != GLFW_MOUSE_BUTTON_LEFT;
    }

    static void clamp(float& value, float min, float max) {
        if (value > max)
            value = max;
        else if (value < min)
            value = min;
    }

    static void mouse_cursor(double x, double y) {
        print("mouse_cursor");

        if (first_mouse) {
            last_x = (float)x;
            last_y = (float)y;
            first_mouse = false;
        }

        float xoffset = (float)x - last_x;
        float yoffset = last_y - (float)y;

        last_x = (float)x;
        last_y = (float)y;

        xoffset *= s_camera.sensitivity;
        yoffset *= s_camera.sensitivity;

        s_camera.yaw += xoffset;
        s_camera.pitch += yoffset;

//        clamp(s_camera.pitch, -s_camera.max_pitch, s_camera.max_pitch);

        glm::vec3 direction;
        direction.x = cos(glm::radians(s_camera.yaw)) * cos(glm::radians(s_camera.pitch));
        direction.y = sin(glm::radians(s_camera.pitch));
        direction.z = sin(glm::radians(s_camera.yaw)) * cos(glm::radians(s_camera.pitch));
        s_camera.front = glm::normalize(direction);
    }

    static void mouse_scroll(double x, double y) {
        print("mouse_scroll()");
        s_camera.fov -= (float) y;
        clamp(s_camera.fov, 1, s_camera.max_fov);
        s_perspective.value = gl::mat_perspective({ s_camera.fov, win::get_aspect_ratio() });
        gl::shader_use(rect_shader);
        gl::shader_set_uniform(rect_shader, s_perspective);
    }

    static gl::triangle_solid triangle_init(u32 i) {
        gl::triangle_solid triangle;
        float offset = i + 1;

        auto& v0 = triangle.v0;
        v0.pos.x *= offset;
        v0.pos.y *= offset;
        v0.pos.z *= offset;

        v0.color.r *= offset;
        v0.color.g *= 0;
        v0.color.b *= 0;

        auto& v1 = triangle.v1;
        v1.pos.x *= offset;
        v1.pos.y *= offset;
        v1.pos.z *= offset;

        v1.color.r *= 0;
        v1.color.g *= offset;
        v1.color.b *= 0;

        auto& v2 = triangle.v2;
        v2.pos.x *= offset;
        v2.pos.y *= offset;
        v2.pos.z *= offset;

        v2.color.r *= 0;
        v2.color.g *= 0;
        v2.color.b *= offset;

        return triangle;
    }

    static gl::rect_uv rect_init(u32 i) {
        gl::rect_uv rect;
        rect.vertices.v0.uv = { 1, 1 };
        rect.vertices.v1.uv = { 1, 0 };
        rect.vertices.v2.uv = { 0, 0 };
        rect.vertices.v3.uv = { 0, 1 };
        return rect;
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

        triangles = gl::triangles_solid_init(triangle_shader, triangle_vao, triangle_vbo, 1, triangle_init);
        rects = gl::rects_uv_init(rect_shader, rect_vao, rect_vbo, rect_ibo, 1, rect_init);

        wall_tbo = gl::texture2d_init("images/wall.jpg");
        smile_tbo = gl::texture2d_init("images/smile.png", true);

        s_perspective.value = gl::mat_perspective({ s_camera.fov, win::get_aspect_ratio() });
        gl::shader_use(rect_shader);
        gl::shader_set_uniform(rect_shader, s_perspective);
        gl::shader_set_model(rect_shader, s_rect_transform);

        gl::polygon_fill();
        glEnable(GL_DEPTH_TEST);
    }

    static void free() {
        gl::free(triangle_shader, triangle_vao, triangle_vbo);
        gl::free(rect_shader, rect_vao, rect_vbo, rect_ibo);
        win::free();
    }

    static void poll_events() {
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
    }

    void run() {
        init();

        while (s_running) {
            float t = glfwGetTime();
            s_running = win::is_open();
            poll_events();
            win::update();
            gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//            gl::draw_triangle(triangle_shader, triangle_vao);
//            gl::draw_rect(rect_shader, rect_vao, rect_ibo);
//            gl::draw_triangles(triangle_shader, triangle_vao, triangles.size());
//            s_rect_color.value.r = value;
//            gl::shader_set_uniform(rect_shader, s_rect_color);
//            gl::shader_use(triangle_shader);
//            gl::draw_triangles(triangle_vao, triangles.size());
//
//            s_rect_transform_data.rotation.z += -abs(log(s_dt));
//            s_rect_transform_data.scale.x += sin(t*2 + 2) / 100.0f;
//            s_rect_transform_data.scale.y += sin(t*2 + 2) / 100.0f;

//            s_rect_model.value = glm::rotate(s_rect_model.value, t * glm::radians(0.5f), { 0.5, 1.0, 0.0 });

            gl::shader_use(rect_shader);
            gl::shader_set_camera(rect_shader, s_camera);

            mix_factor.value = sin(2*t);

            gl::shader_set_uniform(rect_shader, wall_sampler);
            gl::texture_bind(wall_tbo, GL_TEXTURE_2D, wall_sampler.value);
            gl::shader_set_uniform(rect_shader, smile_sampler);
            gl::texture_bind(smile_tbo, GL_TEXTURE_2D, smile_sampler.value);
            gl::shader_set_uniform(rect_shader, mix_factor);

            gl::draw_rects(rect_vao, rect_ibo, rects.size());

            s_dt = ((float)glfwGetTime() - t) * 1000;
        }

        free();
    }

}