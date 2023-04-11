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
#include <entity_picker.h>
#include <terrain.h>

namespace gl {

    static bool running = true;
    static float dt = 6;
    static float begin_time = 0;

    static ecs::Scene scene;

    static Camera camera;

    static LightPresent point_light_present;

    static Terrain terrain = {
            &scene,
            { 0, -5, 4 },
            { 0, 0, 0 },
            { 1, 1, 1 },
            32
    };

    static io::Model backpack_model;
    static PBR_Entity backpack = {
            &scene,
            { -3, 1.5, 5 },
            { -90, 0, 0 },
            { 1, 1, 1 }
    };

    static int enable_hdr = true;
    static int enable_blur = false;
    static int enable_bloom = true;

    static SSAO_Pass ssao_pass;
    static int enable_ssao = true;

    static PBR_Pipeline pbr_pipeline;

    static HDR_Renderer hdr_renderer;

    static Bloom bloom;

    static Animator human_animator;
    static io::SkeletalModel human_model;
    static PBR_Entity human = {
            &scene,
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };

    static SphereTBN rock_sphere_geometry = { 128, 128 };
    static PBR_Entity rock_sphere = {
            &scene,
            { 0, 1.5, 0 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };

    static PBR_Entity wood_sphere = {
            &scene,
            { 0, 1.5, 4 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };

    static PBR_Entity metal_sphere = {
            &scene,
            { 0, 1.5, 8 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };

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

    static void geometry_debug_enabled() {
        terrain.add_component<PolygonVisual>();
        rock_sphere.add_component<PolygonVisual>();
        wood_sphere.add_component<PolygonVisual>();
        metal_sphere.add_component<PolygonVisual>();
        backpack.add_component<PolygonVisual>();

        terrain.add_component<NormalVisual>();
        rock_sphere.add_component<NormalVisual>();
        wood_sphere.add_component<NormalVisual>();
        metal_sphere.add_component<NormalVisual>();
        backpack.add_component<NormalVisual>();
    }

    static void geometry_debug_disabled() {
        terrain.remove_component<PolygonVisual>();
        rock_sphere.remove_component<PolygonVisual>();
        wood_sphere.remove_component<PolygonVisual>();
        metal_sphere.remove_component<PolygonVisual>();
        backpack.remove_component<PolygonVisual>();

        terrain.remove_component<NormalVisual>();
        rock_sphere.remove_component<NormalVisual>();
        wood_sphere.remove_component<NormalVisual>();
        metal_sphere.remove_component<NormalVisual>();
        backpack.remove_component<NormalVisual>();
    }

    static int enable_normal_mapping = true;
    static int enable_parallax_mapping = true;
    static int enable_geometry_debug = false;

    static void key_press(int key) {
        print("key_press(): " << key);

        if (key == KEY::Esc)
            win::close();

        if (key == KEY::N)
            enable_normal_mapping = !enable_normal_mapping;

        if (key == KEY::P) {
            enable_geometry_debug = !enable_geometry_debug;
            if (enable_geometry_debug)
                geometry_debug_enabled();
            else
                geometry_debug_disabled();
        }

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
            rock_sphere.add_component<Outline>();
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
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            win::Cursor mouse_cursor = win::mouse_cursor();
            PBR_Pixel pixel;
            pbr_pipeline.read_pixel(pixel, (int) mouse_cursor.x, (int) mouse_cursor.y);
            print("Read pixel from [x,y] = [" << mouse_cursor.x << "," << mouse_cursor.y << "]");
            EntityPicker entity_picker(&scene, pixel.entity_id);
            print("");
        }
    }

    static void mouse_release(int button) {
        print("mouse_release()");
    }

    static void mouse_cursor(double x, double y) {
        camera.look(x, y);
    }

    static void mouse_scroll(double x, double y) {
        camera.zoom(x, y);
    }

    static void init_scene() {
        pbr_pipeline.scene = &scene;

        terrain.add_component<PBR_Component_Forward>();
        terrain.init();

        rock_sphere.add_component<PBR_Component_Deferred>();
        wood_sphere.add_component<PBR_Component_Deferred>();
        metal_sphere.add_component<PBR_Component_Deferred>();

        backpack.add_component<PBR_Component_Deferred>();

        human.add_component<PBR_SkeletalComponent_Deferred>();
    }

    static void init() {
        win::init({ 0, 0, 800, 600, "Educational Project", win::win_flags::sync });

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

        // setup environment
        pbr_pipeline.env.resolution = { 2048, 2048 };
        pbr_pipeline.env.prefilter_resolution = { 2048, 2048 };
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

        Image shiny_image = ImageReader::read("images/lens_dirt/lens_dirt.png");
        hdr_renderer.shiny.init();
        hdr_renderer.shiny.load(shiny_image);
        shiny_image.free();

        // setup PBR
        pbr_pipeline.init(win::props().width, win::props().height);
        pbr_pipeline.init_hdr_env("images/hdr/Arches_E_PineTree_3k.hdr", true);
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
        backpack_model.generate("models/backpack/backpack.obj");
        backpack_model.init(*backpack.get_component<DrawableElements>());
//        model_shadow.init(model);
        backpack.material()->init(
                true,
                "models/backpack/diffuse.jpg",
                "models/backpack/normal.png",
                null,
                "models/backpack/specular.jpg",
                "models/backpack/roughness.jpg",
                "models/backpack/ao.jpg"
        );
        backpack.material()->metallic_factor = 1;
        backpack.material()->roughness_factor = 1;
        backpack.material()->ao_factor = 1;

        // setup human model
        human_model.generate("models/dancing-stormtrooper/source/silly_dancing.fbx");
        human_model.init(*human.get_component<DrawableElements>());
        human.material()->init(
                false,
                "models/dancing-stormtrooper/textures/Stormtrooper_D.png",
                null,
                null,
                null,
                null,
                null
        );
        human.material()->metallic_factor = 0.5;
        human.material()->roughness_factor = 0.5;
        human.material()->ao_factor = 1.0;
        human_animator = Animator(&human_model.animation);

        // setup horizontal plane
        terrain.material()->init(
                false,
                "images/bumpy-rockface1-bl/albedo.png",
                "images/bumpy-rockface1-bl/normal.png",
                null,
                "images/bumpy-rockface1-bl/metallic.png",
                "images/bumpy-rockface1-bl/roughness.png",
                "images/bumpy-rockface1-bl/ao.png"
        );
        terrain.material()->color = { 1, 1, 1, 1 };
        terrain.material()->metallic_factor = 1;
        terrain.material()->roughness_factor = 1;
        terrain.material()->ao_factor = 1;

        // setup sphere
        SphereTBN sphere_geometry;
        sphere_geometry.init(*wood_sphere.get_component<DrawableElements>());
        sphere_geometry.init(*metal_sphere.get_component<DrawableElements>());
//        SphereTBN sphere_shadow_geometry;
//        sphere_shadow_geometry.init_default();
        // setup rock sphere
        rock_sphere_geometry.init(*rock_sphere.get_component<DrawableElements>());
//        sphere_rock_shadow_geometry.x_segments = 2047;
//        sphere_rock_shadow_geometry.y_segments = 2047;
//        sphere_rock_shadow_geometry.init_default(sphere_rock_shadow);

        {
            rock_sphere.material()->init(
                    false,
                    "images/bumpy-rockface1-bl/albedo.png",
                    "images/bumpy-rockface1-bl/normal.png",
                    null,
                    "images/bumpy-rockface1-bl/metallic.png",
                    "images/bumpy-rockface1-bl/roughness.png",
                    "images/bumpy-rockface1-bl/ao.png"
            );
            rock_sphere.material()->metallic_factor = 1;
            rock_sphere.material()->roughness_factor = 1;
            rock_sphere.material()->ao_factor = 1;
            rock_sphere.material()->color = { 1, 1, 1, 1 };

            // displacement
            {
                // displace rock sphere
                Image rock_height_map = ImageReader::read("images/bumpy-rockface1-bl/height.png");
                rock_height_map.resize(rock_sphere_geometry.x_segments + 1, rock_sphere_geometry.y_segments + 1);

                rock_sphere.add_component<DisplacementTBN>();
                auto* rock_sphere_displacement = rock_sphere.get_component<DisplacementTBN>();
                rock_sphere_displacement->set_origin_vertices(&rock_sphere_geometry.vertices);
                rock_sphere_displacement->scale = 0.125f;
                rock_sphere_displacement->map = HeightMap(rock_height_map);
                rock_sphere_displacement->displace(*rock_sphere.get_component<DrawableElements>());

                rock_height_map.free();

                // displace terrain
                terrain.displace(10.0f, 10, 0, 1, 0.5f);
            }

            wood_sphere.material()->init(
                    false,
                    "images/cheap-plywood1-bl/albedo.png",
                    "images/cheap-plywood1-bl/normal.png",
                    null,
                    "images/cheap-plywood1-bl/metallic.png",
                    "images/cheap-plywood1-bl/roughness.png",
                    "images/cheap-plywood1-bl/ao.png"
            );
            wood_sphere.material()->metallic_factor = 1;
            wood_sphere.material()->roughness_factor = 1;
            wood_sphere.material()->ao_factor = 1;

            metal_sphere.material()->init(
                    false,
                    "images/light-gold-bl/albedo.png",
                    "images/light-gold-bl/normal.png",
                    null,
                    "images/light-gold-bl/metallic.png",
                    "images/light-gold-bl/roughness.png",
                    null
            );
            metal_sphere.material()->metallic_factor = 1;
            metal_sphere.material()->roughness_factor = 1;
            metal_sphere.material()->ao_factor = 1;
        }

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

        Screen::src = pbr_pipeline.render_target();
        pbr_pipeline.update_sunlight();
        pbr_pipeline.update_pointlights();
        pbr_pipeline.update_flashlight();
    }

    static void free() {
        terrain.free();

        pbr_pipeline.env.free();

        pbr_pipeline.free();
        Screen::free();
        hdr_renderer.free();

        wood_sphere.material()->free();
        wood_sphere.drawable()->free();

        metal_sphere.material()->free();
        metal_sphere.drawable()->free();

        rock_sphere.material()->free();
        rock_sphere.drawable()->free();

        Blur::free();

        bloom.free();

        SSAO::free();
        ssao_pass.free();

        point_light_present.free();

        backpack.material()->free();
        backpack.drawable()->free();
        backpack_model.free();

        human.material()->free();
        human.drawable()->free();
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
        rock_sphere.transform()->rotation.y += f;
        wood_sphere.transform()->rotation.y += f * 2;
        metal_sphere.transform()->rotation.y += f * 4;
        human.transform()->rotation.y += f * 4;

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
        if (win::is_key_press(KEY::D1)) {
            Screen::src = pbr_pipeline.render_target();
        }

        else if (win::is_key_press(KEY::D2)) {
            Screen::src = pbr_pipeline.gbuffer().position;
        }

        else if (win::is_key_press(KEY::D3)) {
            Screen::src = pbr_pipeline.gbuffer().normal;
        }

        else if (win::is_key_press(KEY::D4)) {
            Screen::src = pbr_pipeline.gbuffer().albedo;
        }

        else if (win::is_key_press(KEY::D5)) {
            Screen::src = pbr_pipeline.gbuffer().pbr_params;
        }

        else if (win::is_key_press(KEY::D6)) {
            Screen::src = pbr_pipeline.gbuffer().shadow_proj_coords;
        }

        else if (win::is_key_press(KEY::D7)) {
            Screen::src = SSAO::render_target;
        }

        else if (win::is_key_press(KEY::D8)) {
            Screen::src = pbr_pipeline.env.hdr;
        }

        else if (win::is_key_press(KEY::D9)) {
            Screen::src = pbr_pipeline.transparent_buffer().revealage;
        }

        else if (win::is_key_press(KEY::D0)) {
        }
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