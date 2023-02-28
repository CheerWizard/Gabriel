#include <app.h>

#include <window.h>
#include <draw.h>
#include <camera.h>
#include <light.h>
#include <model_loader.h>
#include <outline.h>
#include <shadow.h>
#include <math_functions.h>
#include <ui.h>

#include <map>
#include <random>

namespace app {

    static bool running = true;
    static float dt = 6;
    static float begin_time = 0;

    static u32 material_shader;
    static u32 lights_ubo;
    static u32 sunlight_ubo;

    static gl::camera s_camera;

    static gl::light_present point_light_present;
    static std::array<gl::light_point, 4> point_lights;

    static gl::light_point sunlight;

    static io::drawable_elements plane;
    static std::vector<gl::transform> plane_transforms = {
            {
                    { 0, -2, 0 },
                    { 90, 180, 0 },
                    { 20, 20, 20 }
            }
    };
    static gl::material plane_material;

    static io::drawable_model backpack;
    static gl::transform backpack_transform = {
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };
    static gl::outline_t backpack_outline;
    static gl::material backpack_material;
    static io::shadow_drawable_model backpack_shadow;

    static gl::drawable_elements window;
    static std::vector<gl::transform> window_transforms = {
            {
                    { 0, 5, 20 },
                    { 0, 0, 0 },
                    { 10, 10, 10 }
            }
    };
    static gl::material window_material;

    static std::map<float, gl::transform> transparent_objects;

    static u32 screen_shader;
    static u32 screen_vao;
    static float screen_gamma = 2.2f;
    static float screen_exposure = 1.0f;
    static gl::color_attachment final_render_target;

    static u32 scene_fbo;
    static std::vector<gl::color_attachment> scene_colors;
    static gl::render_buffer scene_rbo;

    static u32 msaa_fbo;
    static std::vector<gl::color_attachment> msaa_colors;
    static gl::render_buffer msaa_rbo;
    static int msaa = 8;

    static u32 blur_fbo;
    static std::vector<gl::color_attachment> blur_colors;
    static u32 blur_shader;
    static float blur_offset = 1.0 / 300.0;
    static int enable_blur = false;

    static u32 ssao_fbo;
    static std::vector<gl::color_attachment> ssao_colors;
    static u32 ssao_shader;
    static std::vector<glm::vec3> ssao_kernel;
    static std::vector<glm::vec3> ssao_noise;
    static gl::texture ssao_noise_texture;
    static int enable_ssao = false;

    static u32 skybox_shader;
    static gl::drawable_elements skybox;
    static gl::texture skybox_texture;

    static void window_error(int error, const char* msg) {
        print_err("window_error: error=" << error << ", msg=" << msg);
    }

    static void window_close() {
        print("window_close()");
    }

    static void window_resized(int w, int h) {
        print("window_resized()");
    }

    static void framebuffer_resized(int w, int h) {
        gl::camera_resize(s_camera, w, h);

        gl::shader_use(skybox_shader);
        glm::mat4 skybox_perspective = gl::perspective_mat({
            s_camera.fov,
            s_camera.aspect,
            s_camera.z_near,
            s_camera.z_far
        });
        gl::shader_set_uniform4m(skybox_shader, "perspective", skybox_perspective);
    }

    static int enable_normal_mapping = true;
    static int enable_parallax_mapping = true;

    static void key_press(int key) {
        print("key_press()");

        if (key == KEY::Escape)
            win::close();

        if (key == KEY::N) {
            enable_normal_mapping = !enable_normal_mapping;
        }

        if (key == KEY::P) {
            enable_parallax_mapping = !enable_parallax_mapping;
        }

        if (key == KEY::B) {
            enable_blur = !enable_blur;
        }

        if (key == KEY::O) {
            enable_ssao = !enable_ssao;
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

    static void mouse_cursor(double x, double y) {
//        print("mouse_cursor");
        gl::camera_look(s_camera, x, y);
    }

    static void mouse_scroll(double x, double y) {
        print("mouse_scroll()");
        gl::camera_zoom(s_camera, x, y);

        gl::shader_use(skybox_shader);
        glm::mat4 skybox_perspective = gl::perspective_mat({
            s_camera.fov,
            s_camera.aspect,
            s_camera.z_near,
            s_camera.z_far
        });
        gl::shader_set_uniform4m(skybox_shader, "perspective", skybox_perspective);
    }

    static void init() {
        win::init({ 1366, 768, "CGP", true });
//        win::disable_cursor();

        win::event_registry::window_error = window_error;
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

        // setup shaders
        material_shader = gl::shader_init({
            "shaders/material.vert",
            "shaders/material.frag"
        });
        screen_shader = gl::shader_init({
            "shaders/screen.vert",
            "shaders/screen.frag"
        });
        blur_shader = gl::shader_init({
            "shaders/screen.vert",
            "shaders/blur.frag"
        });
        skybox_shader = gl::shader_init({
            "shaders/cubemap.vert",
            "shaders/cubemap.frag"
        });

        // setup screen
        screen_vao = gl::vao_init();

        // setup scene frame
        gl::color_attachment scene_color = { win::props().width, win::props().height };
        scene_color.internal_format = GL_RGBA32F;
        scene_color.data_format = GL_RGBA;
        gl::color_attachment scene_bright_color = scene_color;
        scene_colors.emplace_back(scene_color); // main
        scene_colors.emplace_back(scene_bright_color); // brightness
        scene_rbo = { win::props().width, win::props().height };
        scene_fbo = gl::fbo_init(&scene_colors, null, null, &scene_rbo);

        // setup MSAA frame
        gl::color_attachment msaa_color = { win::props().width, win::props().height, msaa };
        msaa_color.internal_format = GL_RGBA32F;
        msaa_color.data_format = GL_RGBA;
        msaa_color.texture.type = GL_TEXTURE_2D_MULTISAMPLE;
        gl::color_attachment msaa_bright_color = msaa_color;
        msaa_colors.emplace_back(msaa_color); // main
        msaa_colors.emplace_back(msaa_bright_color); // brightness
        msaa_rbo = { win::props().width, win::props().height, msaa };
        msaa_fbo = gl::fbo_init(&msaa_colors, null, null, &msaa_rbo);

        // setup Blur frame
        gl::color_attachment blur_color = { win::props().width, win::props().height };
        blur_color.internal_format = GL_RGBA32F;
        blur_color.data_format = GL_RGBA;
        blur_colors.emplace_back(blur_color);
        blur_fbo = gl::fbo_init(&blur_colors, null, null, null);

        // setup SSAO
        // SSAO kernels
        std::uniform_real_distribution<float> random_floats(0.0, 1.0);
        std::default_random_engine generator;
        for (u32 i = 0; i < 64; i++) {
            glm::vec3 sample(
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator)
            );
            sample = glm::normalize(sample);
            sample *= random_floats(generator);
            float scale = (float) i / 64.0;
            scale = gl::lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssao_kernel.emplace_back(sample);
            ssao_kernel.emplace_back(sample);
        }
        // SSAO noise
        for (u32 i = 0; i < 16; i++) {
            glm::vec3 noise(
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    0.0f
            );
            ssao_noise.emplace_back(noise);
        }
        // SSAO noise texture
        gl::texture_data ssao_noise_image = { 4, 4, GL_RGBA32F, GL_RGB, GL_FLOAT, &ssao_noise[0] };
        gl::texture_2d_params ssao_noise_params;
        ssao_noise_params.generate_mipmap = false;
        ssao_noise_params.min_filter = GL_NEAREST;
        ssao_noise_params.mag_filter = GL_NEAREST;
        ssao_noise_params.s = GL_REPEAT;
        ssao_noise_params.t = GL_REPEAT;
        gl::texture2d_init(ssao_noise_texture, ssao_noise_image);
        // SSAO frame
        gl::color_attachment ssao_color = { win::props().width, win::props().height };
        ssao_color.internal_format = GL_RED;
        ssao_color.data_format = GL_RED;
        ssao_color.primitive_type = GL_FLOAT;
        ssao_color.min_filter = GL_NEAREST;
        ssao_color.mag_filter = GL_NEAREST;
        ssao_colors.emplace_back(ssao_color);
        ssao_fbo = gl::fbo_init(&ssao_colors, null, null, null);

        // setup main camera
        s_camera = gl::camera_init(0);
        s_camera.max_pitch = 180;

        // setup skybox
        gl::cube_default_init(skybox, {});
        gl::texture_cube_init(skybox_texture, {
                gl::texture_face { "images/skybox/right.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_X },
                gl::texture_face { "images/skybox/left.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
                gl::texture_face { "images/skybox/top.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
                gl::texture_face { "images/skybox/bottom.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
                gl::texture_face { "images/skybox/front.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
                gl::texture_face { "images/skybox/back.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z },
        });
        gl::shader_use(skybox_shader);
        glm::mat4 skybox_perspective = gl::perspective_mat({
            s_camera.fov,
            s_camera.aspect,
            s_camera.z_near,
            s_camera.z_far
        });
        gl::shader_set_uniform4m(skybox_shader, "perspective", skybox_perspective);

        // setup sunlight
        sunlight_ubo = gl::ubo_init(1, sizeof(sunlight));
        sunlight.position = { 0, 5, 0 };
        sunlight.diffuse = { 100, 100, 100 };
        gl::ubo_update(sunlight_ubo, {
                0,
                sizeof(sunlight),
                sunlight.to_float()
        });
        // setup lights
        lights_ubo = gl::ubo_init(2, sizeof(point_lights));
        point_lights[0].position = { -5, 2, -5 };
        point_lights[1].position = { 5, 2, -5 };
        point_lights[2].position = { -5, 2, 5 };
        point_lights[3].position = { 5, 2, 5 };
        for (int i = 0 ; i < point_lights.size() ; i++) {
            auto& point_light = point_lights[i];
            point_light.ambient = { 0.2f, 0.2f, 0.2f };
            point_light.diffuse = { 0, 0, 1 };
            point_light.specular = {0, 0, 1 };
            gl::ubo_update(lights_ubo, {
                static_cast<long long>(i * sizeof(point_light)),
                sizeof(point_light),
                point_light.to_float()
            });
        }

        // setup light presentation
        point_light_present.color = { 0, 0, 1, 0.5 };
        gl::light_present_init(&point_light_present);

        // setup shadow
        gl::point_shadow_init({ 1024, 1024 }, sunlight.position);

        backpack_outline = gl::outline_init();

        backpack = io::model_init("models/backpack/backpack.obj");
        backpack_shadow = gl::shadow_model_init(backpack);

        gl::rect_tbn_init(plane);
        plane_material = gl::material_init(
            "images/brickwall/diffuse.jpg",
            null,
            null,
            "images/brickwall/normal.jpg",
            "images/brickwall/parallax.jpg"
        );

        gl::rect_tbn_init(window);
        window_material = gl::material_init(
            "images/window.png"
        );
        gl::texture_2d_params window_material_diff_params;
        window_material_diff_params.s = GL_CLAMP_TO_EDGE;
        window_material_diff_params.t = GL_CLAMP_TO_EDGE;
        gl::texture_update(window_material.diffuse, window_material_diff_params);

        gl::shader_use(material_shader);
        backpack_material = backpack.model.materials[backpack.model.meshes.front().material_index];
        gl::material_update(material_shader, backpack_material);

        gl::shader_use(blur_shader);
        gl::shader_set_uniformf(blur_shader, "offset", blur_offset);

        gl::shader_use(0);
        gl::texture_unbind();
    }

    static void free() {
        gl::shader_free(skybox_shader);
        gl::drawable_free(skybox);
        gl::texture_free(skybox_texture.id);

        gl::shader_free(screen_shader);
        gl::vao_free(screen_vao);

        gl::shader_free(blur_shader);
        gl::fbo_free(blur_fbo);
        gl::fbo_free_attachment(blur_colors);

        gl::shader_free(ssao_shader);
        gl::fbo_free(ssao_fbo);
        gl::fbo_free_attachment(ssao_colors);
        gl::texture_free(ssao_noise_texture.id);

        gl::point_shadow_free();
        gl::direct_shadow_free();

        gl::fbo_free(msaa_fbo);
        gl::fbo_free_attachment(msaa_rbo);
        gl::fbo_free_attachment(msaa_colors);

        gl::fbo_free(scene_fbo);
        gl::fbo_free_attachment(scene_rbo);
        gl::fbo_free_attachment(scene_colors);

        gl::drawable_free(window);
        gl::material_free(window_material);

        gl::drawable_free(plane);
        gl::material_free(plane_material);

        gl::shader_free(material_shader);
        gl::ubo_free(lights_ubo);
        gl::ubo_free(sunlight_ubo);

        gl::outline_free();

        gl::light_present_free();

        io::model_free(backpack);
        io::shadow_model_free(backpack_shadow);

        gl::camera_free();

        win::free();
    }

    static void camera_control_update() {
        float camera_speed = s_camera.speed / dt;
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

        gl::camera_view_update(s_camera);
        gl::camera_view_position_update(material_shader, s_camera.position);
    }

    static void simulate() {
        float t = begin_time;
        camera_control_update();
        // rotate object each frame
//                float f = 0.2f;
//                backpack_transform.rotation.y += f;
        // translate point light up/down
        for (auto& point_light : point_lights) {
            point_light.position.y = 4 * sin(t/2) + 2;
        }
        // sorting transparent drawables
        transparent_objects.clear();
        for (auto& window_transform : window_transforms) {
            float distance = glm::length(s_camera.position - window_transform.translation);
            transparent_objects[distance] = window_transform;
        }

        backpack_material.enable_normal = enable_normal_mapping;
        plane_material.enable_normal = enable_normal_mapping;
        window_material.enable_normal = enable_normal_mapping;

        plane_material.enable_parallax = enable_parallax_mapping;
    }

    static void render_screen_ui() {
        ui::theme_selector("Theme");
        ui::slider("Gamma", &screen_gamma, 1.2, 3.2, 0.1);
        ui::slider("Exposure", &screen_exposure, 0, 5.0, 0.01);
        ui::checkbox("Normal Mapping", &enable_normal_mapping);
        ui::checkbox("Parallax Mapping", &enable_parallax_mapping);
        ui::checkbox("Blur", &enable_blur);
        ui::checkbox("SSAO", &enable_ssao);
    }

    static void render_materials_ui() {
        ui::image(window_material.diffuse.id, 128, 128);
    }

    static void render_ui() {
        ui::begin();

        ui::window("Screen", render_screen_ui);
        ui::window("Materials", render_materials_ui);

        ui::end();
    }

    static void render_scene() {
        // render shadows
        {
            // render scene into direct shadows
//                gl::direct_shadow_begin();
//                {
//                    gl::direct_shadow_update(s_light_dir.direction);
//                    for (auto& window_transform : window_transforms) {
//                        gl::direct_shadow_draw(window_transform, window_shadow);
//                    }
//                    gl::direct_shadow_draw(backpack_transform, backpack_shadow.elements);
//                }
            // render scene into point shadows
            gl::point_shadow_begin();
            {
                gl::point_shadow_update(sunlight.position);
                gl::point_shadow_draw(backpack_transform, backpack_shadow.elements);
                for (auto& point_light : point_lights) {
                    gl::point_shadow_update(point_light.position);
                    gl::point_shadow_draw(backpack_transform, backpack_shadow.elements);
                }
            }
        }
        if (msaa > 1) {
            gl::shadow_end(msaa_fbo, win::props().width, win::props().height);
        } else {
            gl::shadow_end(scene_fbo, win::props().width, win::props().height);
        }

        // render scene into MSAA fbo
        gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glStencilMask(0x00);

        // render skybox
        {
            glDepthFunc(GL_LEQUAL);
            gl::shader_use(skybox_shader);
            glm::mat4 view = glm::mat4(glm::mat3(gl::view_mat(s_camera)));
            gl::shader_set_uniform4m(skybox_shader, "view", view);
            gl::texture_update(skybox_shader, skybox_texture);
            gl::draw(skybox);
            glDepthFunc(GL_LESS);
        }

        // upload shadow maps
        {
            gl::shader_use(material_shader);
//                gl::direct_shadow_update(material_shader);
            gl::point_shadow_update(material_shader);
        }

        // render and upload lights
        {
            // sunlight
            point_light_present.transform.translation = sunlight.position;
            gl::light_present_update();
            gl::ubo_update(sunlight_ubo, { 0, sizeof(sunlight), sunlight.to_float() });
        }
        {
            // point lights
            size_t point_light_size = point_lights.size();
            for (int i = 0 ; i < point_light_size ; i++) {
                auto& point_light = point_lights[i];
                // render presentation
                point_light_present.transform.translation = point_light.position;
                gl::light_present_update();
                // upload data to ubo
                gl::ubo_update(lights_ubo, {
                        static_cast<long long>(i * sizeof(point_light)),
                        sizeof(point_light),
                        point_light.to_float()
                });
            }
        }
        // render default objects
        {
            gl::shader_use(material_shader);
            gl::material_update(material_shader, plane_material);
            for (auto& plane_transform : plane_transforms) {
                gl::transform_update(material_shader, plane_transform);
                gl::draw(plane);
            }
        }

        // render outlined objects
        {
            gl::outline_begin();

            gl::shader_use(material_shader);
            gl::transform_update(material_shader, backpack_transform);
            gl::material_update(material_shader, backpack_material);
            gl::draw(backpack.elements);

            gl::outline_end(backpack_transform, backpack.elements, backpack_outline);
        }

        // render transparent objects
        {
            gl::shader_use(material_shader);
            gl::material_update(material_shader, window_material);
            for (auto it = transparent_objects.rbegin() ; it != transparent_objects.rend() ; it++) {
                gl::transform_update(material_shader, it->second);
                gl::draw(window);
            }
        }

        // MSAA -> scene pass
        if (msaa > 1) {
            int w = win::props().width;
            int h = win::props().height;
            gl::fbo_blit(msaa_fbo, w, h, scene_fbo, w, h, msaa_colors.size());
        }

        // scene -> post effects
        final_render_target = scene_colors[0];

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

        // post effects
        {
            // blur effect
            if (enable_blur) {
                gl::fbo_bind(blur_fbo);
                gl::clear_display(1, 1, 1, 1, GL_COLOR_BUFFER_BIT);
                gl::shader_use(blur_shader);
                gl::texture_update(blur_shader, final_render_target.texture);
                gl::draw_quad(screen_vao);
                final_render_target = blur_colors[0];
            }
        }

        // render into screen
        {
            gl::fbo_unbind();
            gl::clear_display(1, 1, 1, 1, GL_COLOR_BUFFER_BIT);
            gl::shader_use(screen_shader);
            gl::texture_update(screen_shader, final_render_target.texture);
            gl::shader_set_uniformf(screen_shader, "gamma", screen_gamma);
            gl::shader_set_uniformf(screen_shader, "exposure", screen_exposure);
            gl::draw_quad(screen_vao);
        }
    }

    void run() {
        init();

        while (running) {
            begin_time = glfwGetTime();
            running = win::is_open();

            win::poll();

            simulate();

            render_scene();

#ifdef UI
            render_ui();
#endif

            win::swap();

            dt = ((float)glfwGetTime() - begin_time) * 1000;
        }

        free();
    }

}