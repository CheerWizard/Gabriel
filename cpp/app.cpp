#include <app.h>

#include <window.h>
#include <ui.h>

#include <camera.h>
#include <light.h>
#include <model_loader.h>
#include <shadow.h>
#include <math_functions.h>
#include <sphere.h>
#include <ssao.h>
#include <pbr.h>
#include <screen.h>
#include <hdr.h>
#include <blur.h>
#include <bloom.h>
#include <skeletal_animation.h>

namespace gl {

    static bool running = true;
    static float dt = 6;
    static float begin_time = 0;

    static Scene scene;

    static Camera camera;

    static LightPresent point_light_present;

    static Material plane_material;
    static DrawableElements plane_drawable;
    static PBR_Entity<PBR_Component_Deferred> plane;

    static Material backpack_material;
    static io::DrawableModel backpack_model;
    static PBR_Entity<PBR_Component_Deferred> backpack;

    static int enable_hdr = true;
    static int enable_blur = false;
    static int enable_bloom = true;

    static DirectShadow direct_shadow;
    static PointShadow point_shadow;

    static SSAO_Pass ssao_pass;
    static int enable_ssao = true;

    static PBR_Pipeline pbr_pipeline;

    static HDR_Renderer hdr_renderer;

    static Bloom bloom;

    static Material human_material;
    static io::DrawableSkeletalModel human_model;
    static PBR_Entity<PBR_SkeletalComponent_Deferred> human;
    static Animator human_animator;

    static DrawableElements sphere_drawable;

    static Material rock_material;
    static DrawableElements rock_drawable;
    static SphereTBN rock_sphere_geometry;
    static PBR_Entity<PBR_Component_Deferred> rock_sphere;

    static Material wood_material;
    static PBR_Entity<PBR_Component_Deferred> wood_sphere;

    static Material metal_material;
    static PBR_Entity<PBR_Component_Deferred> metal_sphere;

    static int print_limiter = 100;

    static void print_dt() {
        print_limiter++;
        if (print_limiter > 100) {
            print_limiter = 0;
            print("dt: " << dt << " ms" << " FPS: " << 1000 / dt);
        }
    }

    static void window_error(int error, const char* msg) {
        print_err("window_error(): error=" << error << ", msg=" << msg);
    }

    static void window_close() {
        print("window_close()");
    }

    static void window_resized(int w, int h) {
        print("window_resized(): width=" << w << ", height=" << h);
    }

    static void window_positioned(int x, int y) {
        print("window_positioned(): x=" << x << ", y=" << y);
    }

    static void framebuffer_resized(int w, int h) {
        print("framebuffer_resized(): width=" << w << ", height=" << h);

        camera.resize(w, h);
        hdr_renderer.resize(w, h);
        pbr_pipeline.resize(w, h);
        bloom.resize(w, h);
        Blur::resize(w, h);
        SSAO::resize(w, h);
        ssao_pass.resolution = { w, h };
    }

    static int enable_normal_mapping = true;
    static int enable_parallax_mapping = true;

    static void key_press(int key) {
        print("key_press(): " << key);

        if (key == KEY::Esc)
            win::close();

        if (key == KEY::N)
            enable_normal_mapping = !enable_normal_mapping;

        if (key == KEY::P)
            enable_parallax_mapping = !enable_parallax_mapping;

        if (key == KEY::B)
            enable_blur = !enable_blur;

        if (key == KEY::H)
            enable_hdr = !enable_hdr;

        if (key == KEY::O)
            enable_ssao = !enable_ssao;

        if (key == KEY::F) {
            win::toggle_window_mode();
        }

        if (key == KEY::E) {
            Outline outline;
            auto* component = rock_sphere.get_component<PBR_Component_Deferred>();
            rock_sphere.add_component<Outline>(&component->transform, component->drawable);
        }

        if (key == KEY::R) {
            rock_sphere.remove_component<Outline>();
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
        camera.look(x, y);

        if (win::is_key_press(KEY::LeftControl)) {
            PBR_Pixel pixel = pbr_pipeline.read_pixel((int) x, (int) y);
            print("PBR: read pixel from [x,y]=[" << x << "," << y << "]");
            print("PBR: object id=" << pixel.object_id);
        }
    }

    static void mouse_scroll(double x, double y) {
        camera.zoom(x, y);
    }

    static void init_scene() {
        plane = {
                &scene,
                {
                        {0, 0, 4 },
                        {0, 0, 0},
                        {40, 1, 40}
                },
                &plane_material,
                &plane_drawable
        };
        plane.component()->material = &plane_material;
        plane.component()->drawable = &plane_drawable;

        backpack = {
                &scene,
                {
                        { -3, 1.5, 5 },
                        { -90, 0, 0 },
                        { 1, 1, 1 }
                },
                &backpack_material,
                &backpack_model.drawable
        };
        backpack.component()->material = &backpack_material;
        backpack.component()->drawable = &backpack_model.drawable;

        human = {
                &scene,
                {
                        { 0, 0, 0 },
                        { 0, 0, 0 },
                        { 1, 1, 1 }
                },
                &human_material,
                &human_model.drawable
        };
        human.component()->material = &human_material;
        human.component()->drawable = &human_model.drawable;

        rock_sphere = {
                &scene,
                {
                        { 0, 1.3, 0 },
                        { 0, 0, 0 },
                        { 1, 1, 1 }
                },
                &rock_material,
                &rock_drawable
        };
        rock_sphere.component()->material = &rock_material;
        rock_sphere.component()->drawable = &rock_drawable;

        wood_sphere = {
                &scene,
                {
                        { 0, 1.5, 4 },
                        { 0, 0, 0 },
                        { 1, 1, 1 }
                },
                &wood_material,
                &sphere_drawable
        };
        wood_sphere.component()->material = &wood_material;
        wood_sphere.component()->drawable = &sphere_drawable;

        metal_sphere = {
                &scene,
                {
                        { 0, 1.5, 8 },
                        { 0, 0, 0 },
                        { 1, 1, 1 }
                },
                &metal_material,
                &sphere_drawable
        };
        metal_sphere.component()->material = &metal_material;
        metal_sphere.component()->drawable = &sphere_drawable;
    }

    static void init() {
        win::init({ 0, 0, 1920, 1080, "Educational Project", win::win_flags::sync });

        win::event_registry::window_error = window_error;
        win::event_registry::window_close = window_close;
        win::event_registry::window_resized = window_resized;
        win::event_registry::window_positioned = window_positioned;
        win::event_registry::framebuffer_resized = framebuffer_resized;
        win::event_registry::key_press = key_press;
        win::event_registry::key_release = key_release;
        win::event_registry::mouse_press = mouse_press;
        win::event_registry::mouse_release = mouse_release;
        win::event_registry::mouse_cursor = mouse_cursor;
        win::event_registry::mouse_scroll = mouse_scroll;

        win::event_registry::set_callbacks();

        // setup screen
        Screen::init();

        init_scene();

        // setup scene
        pbr_pipeline.scene = &scene;
        // setup environment
        pbr_pipeline.env.resolution = { 2048, 2048 };
        pbr_pipeline.env.prefilter_resolution = { 2048, 2048 };
        pbr_pipeline.env.hdr.init_hdr("images/hdr/Arches_E_PineTree_3k.hdr", true);
        pbr_pipeline.env.init();
        // setup sunlight
        static const float sunlight_intensity = 0.05;
        static const glm::vec3 sunlight_rgb = glm::vec3(237, 213, 158) * sunlight_intensity;
        pbr_pipeline.sunlight.color = { sunlight_rgb, 1 };
        pbr_pipeline.sunlight.direction = { 1, 1, 1, 0 };
        // setup lights
        pbr_pipeline.point_lights[0].position = { -4, 2, 0, 1 };
        pbr_pipeline.point_lights[0].color = { 0, 0, 0, 1 };
        pbr_pipeline.point_lights[1].position = { 4, 3, 0, 1 };
        pbr_pipeline.point_lights[1].color = { 0, 0, 0, 1 };
        pbr_pipeline.point_lights[2].position = { -4, 4, 8, 1 };
        pbr_pipeline.point_lights[2].color = { 0, 0, 0, 1 };
        pbr_pipeline.point_lights[3].position = { 4, 5, 8, 1 };
        pbr_pipeline.point_lights[3].color = { 0, 0, 0, 1 };
        // setup flashlight
        pbr_pipeline.flashlight.position = { camera.position, 0 };
        pbr_pipeline.flashlight.direction = { camera.front, 0 };
        pbr_pipeline.flashlight.color = { 0, 0, 0, 1 };

        // setup HDR
        hdr_renderer.init(win::props().width, win::props().height);
        hdr_renderer.set_exposure(1);
        hdr_renderer.shiny.init("images/lens_dirt/lens_dirt.png");

        // setup PBR
        pbr_pipeline.init(win::props().width, win::props().height);
        pbr_pipeline.generate_env();

        // setup Blur
        Blur::init(win::props().width, win::props().height);

        // setup Bloom
        bloom.resolution = { win::props().width, win::props().height };
        bloom.bloom_strength = 0.04;
        bloom.init();

        // setup SSAO
        SSAO::init(win::props().width, win::props().height);
        ssao_pass.resolution = { win::props().width, win::props().height };
        ssao_pass.init();
        pbr_pipeline.set_ssao_pass(&ssao_pass);

        // setup main camera
        camera.init(0, win::get_aspect_ratio());
        camera.max_pitch = 180;
        camera.position = { -5, 2, 10 };

        // setup light presentation
        point_light_present.init();

        // setup 3D model
        backpack_model.init("models/backpack/backpack.obj");
//        model_shadow.init(model);
        auto* plane_component = plane.component();
        auto* backpack_component = backpack.component();
        backpack.component()->material->init(
                true,
                "models/backpack/diffuse.jpg",
                "models/backpack/normal.png",
                null,
                "models/backpack/specular.jpg",
                "models/backpack/roughness.jpg",
                "models/backpack/ao.jpg"
        );
        backpack.component()->material->metallic_factor = 1;
        backpack.component()->material->roughness_factor = 1;
        backpack.component()->material->ao_factor = 1;

        // setup human model
        human_model.init("models/dancing-stormtrooper/source/silly_dancing.fbx");
        human.component()->material->init(
                false,
                "models/dancing-stormtrooper/textures/Stormtrooper_D.png",
                null,
                null,
                null,
                null,
                null
        );
        human.component()->material->metallic_factor = 0.5;
        human.component()->material->roughness_factor = 0.5;
        human.component()->material->ao_factor = 1.0;
        human_animator = Animator(&human_model.model.animation);

        // setup sphere
        SphereTBN sphere_geometry;
        sphere_geometry.init_tbn(sphere_drawable);
//        SphereTBN sphere_shadow_geometry;
//        sphere_shadow_geometry.init_default();
        // setup rock sphere
        rock_sphere_geometry.x_segments = 2047;
        rock_sphere_geometry.y_segments = 2047;
        rock_sphere_geometry.init_tbn(rock_drawable);
//        sphere_rock_shadow_geometry.x_segments = 2047;
//        sphere_rock_shadow_geometry.y_segments = 2047;
//        sphere_rock_shadow_geometry.init_default(sphere_rock_shadow);

        {
            rock_sphere.component()->material->init(
                    false,
                    "images/bumpy-rockface1-bl/albedo.png",
                    "images/bumpy-rockface1-bl/normal.png",
                    null,
                    "images/bumpy-rockface1-bl/metallic.png",
                    "images/bumpy-rockface1-bl/roughness.png",
                    "images/bumpy-rockface1-bl/ao.png"
            );
            rock_sphere.component()->material->metallic_factor = 1;
            rock_sphere.component()->material->roughness_factor = 1;
            rock_sphere.component()->material->ao_factor = 1;
            rock_sphere.component()->material->color = { 1, 1, 1, 1 };

            rock_sphere_geometry.displace(*rock_sphere.component()->drawable, "images/bumpy-rockface1-bl/height.png", false, 3.0f);
//            sphere_rock_shadow_geometry.displace(sphere_rock_shadow, "images/bumpy-rockface1-bl/height.png", false, 3.0f, 0, [](gl::VertexDefault& V) {});

            wood_sphere.component()->material->init(
                    false,
                    "images/cheap-plywood1-bl/albedo.png",
                    "images/cheap-plywood1-bl/normal.png",
                    null,
                    "images/cheap-plywood1-bl/metallic.png",
                    "images/cheap-plywood1-bl/roughness.png",
                    "images/cheap-plywood1-bl/ao.png"
            );
            wood_sphere.component()->material->metallic_factor = 1;
            wood_sphere.component()->material->roughness_factor = 1;
            wood_sphere.component()->material->ao_factor = 1;

            metal_sphere.component()->material->init(
                    false,
                    "images/light-gold-bl/albedo.png",
                    "images/light-gold-bl/normal.png",
                    null,
                    "images/light-gold-bl/metallic.png",
                    "images/light-gold-bl/roughness.png",
                    null
            );
            metal_sphere.component()->material->metallic_factor = 1;
            metal_sphere.component()->material->roughness_factor = 1;
            metal_sphere.component()->material->ao_factor = 1;
        }

        // setup horizontal plane
        CubeTBN plane_geometry;
        plane_geometry.init_tbn(*plane.component()->drawable);
        plane.component()->material->color = {1, 1, 1, 1 };
        plane.component()->material->metallic_factor = 0;
        plane.component()->material->roughness_factor = 0;
        plane.component()->material->ao_factor = 1;

        // render_deferred Shadow pass
//        {
//            // render_deferred direct shadow
//            direct_shadow.begin();
//            {
//                direct_shadow.direction = sunlight.direction;
//                direct_shadow.update();
//            }
//            direct_shadow.end();
//            // render_deferred point shadow
//            point_shadow.begin();
//            {
//                for (auto& point_light : point_lights) {
//                    point_shadow.position = point_light.position;
//                    for (auto& sphere_transform : sphere_transforms) {
//                        point_shadow.draw(sphere_transform, sphere_shadow);
//                    }
//                }
//            }
//            point_shadow.end();
//        }

        // upload shadow maps
//        {
//            pbr_shader.use();
//            direct_shadow.update_light_space(pbr_shader);
//            direct_shadow.update_depth_map(pbr_shader);
//            point_shadow.update(pbr_shader);
//
//            pbr_material_shader.use();
//            direct_shadow.update_light_space(pbr_material_shader);
//
//            pbr_light_shader.use();
//            direct_shadow.update_depth_map(pbr_light_shader);
//        }

        Texture::unbind();
        Shader::stop();
        Screen::src = pbr_pipeline.render_target();

        pbr_pipeline.update_sunlight();
        pbr_pipeline.update_pointlights();
        pbr_pipeline.update_flashlight();
    }

    static void free() {
        pbr_pipeline.env.free();

        pbr_pipeline.free();
        Screen::free();
        hdr_renderer.free();

        wood_sphere.component()->material->free();
        wood_sphere.component()->drawable->free();

        metal_sphere.component()->material->free();
        metal_sphere.component()->drawable->free();

        rock_sphere.component()->material->free();
        rock_sphere.component()->drawable->free();

        Blur::free();

        bloom.free();

        SSAO::free();
        ssao_pass.free();

        plane.component()->material->free();
        plane.component()->drawable->free();

        point_light_present.free();

        backpack.component()->material->free();
        backpack.component()->drawable->free();
        backpack_model.free();

        human.component()->material->free();
        human.component()->drawable->free();
        human_model.free();

        camera.free();

        win::free();
    }

    static void simulate() {
        float t = begin_time;

        camera.move(dt);
        pbr_pipeline.set_camera_pos(camera.position);

        // bind flashlight to camera
        pbr_pipeline.flashlight.position = { camera.position, 0 };
        pbr_pipeline.flashlight.direction = { camera.front, 0 };

        // rotate object each frame
        float f = 0.05f;
        rock_sphere.component()->transform.rotation.y += f;
        wood_sphere.component()->transform.rotation.y += f * 2;
        metal_sphere.component()->transform.rotation.y += f * 4;
        human.component()->transform.rotation.y += f * 4;

        // translate point lights up/down
//        for (auto& point_light : point_lights) {
//            point_light.position.y = 5 * sin(t/5) + 2;
//        }

        // skeletal animations
        {
            // animate human
//            human_animator.update(dt / 1000.0f);
//            pbr.update_bones(human_animator.bone_transforms);
        }
    }

    static void render_screen_ui() {
        ui::theme_selector("Theme");
        ui::checkbox("HDR", &enable_hdr);
        ui::slider("Gamma", &Screen::gamma, 1.2, 3.2, 0.1);
        ui::slider("Exposure", &hdr_renderer.exposure, 0, 5.0, 0.01);
        ui::checkbox("Normal Mapping", &enable_normal_mapping);
        ui::checkbox("Parallax Mapping", &enable_parallax_mapping);
        ui::checkbox("Blur", &enable_blur);
        ui::checkbox("Bloom", &enable_bloom);
        ui::checkbox("SSAO", &enable_ssao);

        ui::slider("Sunlight X", &pbr_pipeline.sunlight.direction.x, -100, 100, 1);
        ui::slider("Sunlight Y", &pbr_pipeline.sunlight.direction.y, -100, 100, 1);
        ui::slider("Sunlight Z", &pbr_pipeline.sunlight.direction.z, -100, 100, 1);
        ui::color_picker("Sunlight Color", pbr_pipeline.sunlight.color);

        ui::slider("PointLight_1 X", &pbr_pipeline.point_lights[0].position.x, -25, 25, 1);
        ui::slider("PointLight_1 Y", &pbr_pipeline.point_lights[0].position.y, -25, 25, 1);
        ui::slider("PointLight_1 Z", &pbr_pipeline.point_lights[0].position.z, -25, 25, 1);
        ui::color_picker("PointLight_1 Color", pbr_pipeline.point_lights[0].color);

        ui::slider("PointLight_2 X", &pbr_pipeline.point_lights[1].position.x, -25, 25, 1);
        ui::slider("PointLight_2 Y", &pbr_pipeline.point_lights[1].position.y, -25, 25, 1);
        ui::slider("PointLight_2 Z", &pbr_pipeline.point_lights[1].position.z, -25, 25, 1);
        ui::color_picker("PointLight_2 Color", pbr_pipeline.point_lights[1].color);

        ui::slider("PointLight_3 X", &pbr_pipeline.point_lights[2].position.x, -25, 25, 1);
        ui::slider("PointLight_3 Y", &pbr_pipeline.point_lights[2].position.y, -25, 25, 1);
        ui::slider("PointLight_3 Z", &pbr_pipeline.point_lights[2].position.z, -25, 25, 1);
        ui::color_picker("PointLight_3 Color", pbr_pipeline.point_lights[2].color);

        ui::slider("PointLight_4 X", &pbr_pipeline.point_lights[3].position.x, -25, 25, 1);
        ui::slider("PointLight_4 Y", &pbr_pipeline.point_lights[3].position.y, -25, 25, 1);
        ui::slider("PointLight_4 Z", &pbr_pipeline.point_lights[3].position.z, -25, 25, 1);
        ui::color_picker("PointLight_4 Color", pbr_pipeline.point_lights[3].color);
    }

    static void render_ui() {
        ui::begin();
        ui::window("Screen", render_screen_ui);
        ui::end();
    }

    static void postfx_rendering() {
        // Bloom effect
        if (enable_bloom) {
            bloom.src = Screen::src;
            bloom.render();
            Screen::src = bloom.render_target;
        }
        // HDR effect
        if (enable_hdr) {
            hdr_renderer.src = Screen::src;
            hdr_renderer.render();
            Screen::src = hdr_renderer.render_target;
        }
        // Blur effect
        if (enable_blur) {
            Blur::src = Screen::src;
            Blur::render();
            Screen::src = Blur::render_target;
        }
    }

    static void debug_screen_update() {
        if (win::is_key_press(KEY::D1))
            Screen::src = pbr_pipeline.render_target();

        else if (win::is_key_press(KEY::D2))
            Screen::src = pbr_pipeline.gbuffer().position;

        else if (win::is_key_press(KEY::D3))
            Screen::src = pbr_pipeline.gbuffer().normal;

        else if (win::is_key_press(KEY::D4))
            Screen::src = pbr_pipeline.gbuffer().albedo;

        else if (win::is_key_press(KEY::D5))
            Screen::src = pbr_pipeline.gbuffer().pbr_params;

        else if (win::is_key_press(KEY::D6))
            Screen::src = pbr_pipeline.gbuffer().shadow_proj_coords;

        else if (win::is_key_press(KEY::D7))
            Screen::src = SSAO::render_target;

        else if (win::is_key_press(KEY::D8))
            Screen::src = pbr_pipeline.env.hdr;

        else if (win::is_key_press(KEY::D9))
            Screen::src = pbr_pipeline.transparent_buffer().revealage;
    }

    static void render() {
        pbr_pipeline.render();
        Screen::src = pbr_pipeline.render_target();
        postfx_rendering();
        debug_screen_update();
        Screen::render();
    }

    App::App() {
        init();
    }

    App::~App() {
        free();
    }

    void App::run() {
        while (running) {
            begin_time = glfwGetTime();
            running = win::is_open();

            win::poll();

            simulate();

            render();

#ifdef UI
            render_ui();
#endif

            win::swap();

            dt = ((float)glfwGetTime() - begin_time) * 1000;

            print_dt();
        }
    }
}