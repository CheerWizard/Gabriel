#include <app.h>

namespace gl {

    App::App() {
        init_window();
        init_pipeline();
        init_camera();
        init_light();
        init_scene();
        init_text();
    }

    App::~App() {
        free();
    }

    void App::run() {
        while (running) {
            begin_time = glfwGetTime();
            running = Window::is_open();

            Window::poll();

            simulate();

            render();

#ifdef UI
            render_ui();
#endif

            Window::swap();

            delta_time = ((float)glfwGetTime() - begin_time) * 1000;

            print_dt();
        }
    }

    void App::init_window() {
        Window::init({ 0, 0, 800, 600, "Educational Project" });

        EventRegistry::window_error = [this](int code, const char* msg) { window_error(code, msg); };
        EventRegistry::window_close = [this]() { window_close(); };
        EventRegistry::window_resized = [this](int w, int h) { window_resized(w, h); };
        EventRegistry::window_positioned = [this](int x, int y) { window_positioned(x, y); };
        EventRegistry::framebuffer_resized = [this](int w, int h) { framebuffer_resized(w, h); };
        EventRegistry::key_press = [this](int key) { key_press(key); };
        EventRegistry::key_release = [this](int key) { key_release(key); };
        EventRegistry::mouse_press = [this](int mouse) { mouse_press(mouse); };
        EventRegistry::mouse_release = [this](int mouse) { mouse_release(mouse); };
        EventRegistry::mouse_cursor = [this](double x, double y) { mouse_cursor(x, y); };
        EventRegistry::mouse_scroll = [this](double x, double y) { mouse_scroll(x, y); };

        EventRegistry::set_callbacks();
    }

    void App::init_scene() {
        backpack = {
                &scene,
                { -3, 1.5, 5 },
                { -90, 0, 0 },
                { 1, 1, 1 }
        };

        terrain = {
                &scene,
                { 0, -15, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 },
                1024
        };

        human = {
                &scene,
                { 0, 0, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        rock_sphere = {
                &scene,
                { 0, 1.5, 0 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        wood_sphere = {
                &scene,
                { 0, 1.5, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        metal_sphere = {
                &scene,
                { 0, 1.5, 8 },
                { 0, 0, 0 },
                { 1, 1, 1 }
        };

        entity_control = new EntityControl(&scene, &camera);

        terrain.add_component<PBR_Component_Forward>();
        terrain.add_component<Selectable>();
        terrain.add_component<Draggable>();
        terrain.init();

        rock_sphere.add_component<PBR_Component_ForwardCull>();
        rock_sphere.add_component<Selectable>(entity_selected);
        rock_sphere.add_component<Draggable>(entity_dragged);
        rock_sphere.add_component<SphereCollider>(rock_sphere.transform()->translation, 3.0f);

        wood_sphere.add_component<PBR_Component_ForwardCull>();
        wood_sphere.add_component<Selectable>(entity_selected);
        wood_sphere.add_component<Draggable>(entity_dragged);

        metal_sphere.add_component<PBR_Component_ForwardCull>();
        metal_sphere.add_component<Selectable>(entity_selected);
        metal_sphere.add_component<Draggable>(entity_dragged);

        backpack.add_component<PBR_Component_ForwardCull>();
        backpack.add_component<Selectable>(entity_selected);
        backpack.add_component<Draggable>(entity_dragged);

        human.add_component<PBR_SkeletalComponent_ForwardCull>();
        human.add_component<Selectable>(entity_selected);
        human.add_component<Draggable>(entity_dragged);

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
                null,
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
        SphereTBN sphere_geometry = { 64, 64 };
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
                rock_sphere_displacement->scale = 1.0f;
                rock_sphere_displacement->map = HeightMap(rock_height_map);
                rock_sphere_displacement->displace(*rock_sphere.get_component<DrawableElements>());

                rock_height_map.free();

                // displace terrain
                Image terrain_height_map = ImageReader::read("images/terrain/earth_heightmap.png");
                terrain_height_map.resize(terrain.size(), terrain.size());

                terrain.displace_with(HeightMap(terrain_height_map), 200);
                image_mixer.displacement_map = &terrain.displacement()->map;
                image_mixer.add_image({ -100, -50, 0 }, "images/terrain/rock_tile.png");
                image_mixer.add_image({ 0, 10, 20 }, "images/terrain/sand_tile.jpg");
                image_mixer.add_image({ 20, 45, 70 }, "images/terrain/grass_tile.png");
                image_mixer.add_image({ 70, 77, 85 }, "images/terrain/rock_tile.png");
                image_mixer.add_image({ 85, 90, 100 }, "images/terrain/snow_tile.png");
                image_mixer.mix(terrain.size(), terrain.size());
                ImageWriter::write("images/terrain/mixed_image.png", image_mixer.mixed_image);

                terrain_height_map.free();

                ImageParams params;
                params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
                terrain.material()->albedo.init();
                terrain.material()->albedo.load(image_mixer.mixed_image, params);
                terrain.material()->enable_albedo = terrain.material()->albedo.id != invalid_image_buffer;
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
    }

    void App::init_pipeline() {
        screen_renderer = new ScreenRenderer();

        ui_pipeline = new UI_Pipeline(&scene);

        debug_control_pipeline = new DebugControlPipeline(&scene);

        pbr_pipeline = new PBR_Pipeline(&scene, Window::get_width(), Window::get_height());

        hdr_renderer = new HdrRenderer(Window::get_width(), Window::get_height());
        Image shiny_image = ImageReader::read("images/lens_dirt/lens_dirt.png");
        ImageBuffer shiny_buffer;
        shiny_buffer.init();
        shiny_buffer.load(shiny_image);
        shiny_image.free();
        hdr_renderer->get_params().shiny_buffer = shiny_buffer;
        hdr_renderer->get_params().exposure.value = 2.0f;
        hdr_renderer->update_exposure();

        blur_renderer = new BlurRenderer(Window::get_width(), Window::get_height());

        bloom_renderer = new BloomRenderer(Window::get_width(), Window::get_height());
    }

    void App::init_camera() {
        camera.z_far = 1000.0f;
        camera.max_pitch = 180;
        camera.position = { -5, 2, 10 };
        camera.init(0, Window::get_width(), Window::get_height());
    }

    void App::init_light() {
        // setup light presentation
        point_light_present.init();
        // setup environment
        pbr_pipeline->env.resolution = { 2048, 2048 };
        pbr_pipeline->env.prefilter_resolution = { 2048, 2048 };
        pbr_pipeline->env.init();
        pbr_pipeline->init_hdr_env("images/hdr/Arches_E_PineTree_3k.hdr", true);
        pbr_pipeline->generate_env();
        // setup sunlight
        static const float sunlight_intensity = 0.05;
        static const glm::vec3 sunlight_rgb = glm::vec3(237, 213, 158) * sunlight_intensity;
        pbr_pipeline->sunlight.color = { sunlight_rgb, 1 };
        pbr_pipeline->sunlight.direction = { 1, 1, 1, 0 };
        // setup lights
        pbr_pipeline->point_lights[0].position = { -4, 2, 0, 1 };
        pbr_pipeline->point_lights[0].color = { 0, 0, 0, 1 };
        pbr_pipeline->point_lights[1].position = { 4, 3, 0, 1 };
        pbr_pipeline->point_lights[1].color = { 0, 0, 0, 1 };
        pbr_pipeline->point_lights[2].position = { -4, 4, 8, 1 };
        pbr_pipeline->point_lights[2].color = { 0, 0, 0, 1 };
        pbr_pipeline->point_lights[3].position = { 4, 5, 8, 1 };
        pbr_pipeline->point_lights[3].color = { 0, 0, 0, 1 };
        // setup flashlight
        pbr_pipeline->flashlight.position = { camera.position, 0 };
        pbr_pipeline->flashlight.direction = { camera.front, 0 };
        pbr_pipeline->flashlight.color = { 0, 0, 0, 1 };
        // update light buffers
        pbr_pipeline->update_sunlight();
        pbr_pipeline->update_pointlights();
        pbr_pipeline->update_flashlight();
    }

    void App::free() {
        FontAtlas::free();

        terrain.free();

        delete screen_renderer;

        delete pbr_pipeline;
        delete ui_pipeline;
        delete debug_control_pipeline;

        delete hdr_renderer;
        delete blur_renderer;
        delete bloom_renderer;

        wood_sphere.material()->free();
        wood_sphere.drawable()->free();

        metal_sphere.material()->free();
        metal_sphere.drawable()->free();

        rock_sphere.material()->free();
        rock_sphere.drawable()->free();

        point_light_present.free();

        backpack.material()->free();
        backpack.drawable()->free();
        backpack_model.free();

        human.material()->free();
        human.drawable()->free();
        human_model.free();

        delete entity_control;

        camera.free();

        Window::free();
    }

    void App::simulate() {
        float t = begin_time;

        camera.move(delta_time);
        pbr_pipeline->set_camera_pos(camera.position);

        // bind flashlight to camera
        pbr_pipeline->flashlight.position = { camera.position, 0 };
        pbr_pipeline->flashlight.direction = { camera.front, 0 };

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

    void App::print_dt() {
        print_limiter++;
        if (print_limiter > 100) {
            print_limiter = 0;
            print("dt: " << delta_time << " ms" << " FPS: " << 1000 / delta_time);
        }
    }

    void App::window_error(int error, const char* msg) {
        print_err("window_error(): error=" << error << ", msg=" << msg);
    }

    void App::window_close() {
        print("window_close()");
    }

    void App::window_resized(int w, int h) {
        print("window_resized(): width=" << w << ", height=" << h);
    }

    void App::window_positioned(int x, int y) {
        print("window_positioned(): x=" << x << ", y=" << y);
    }

    void App::framebuffer_resized(int w, int h) {
        print("framebuffer_resized(): width=" << w << ", height=" << h);

        camera.resize(w, h);
        pbr_pipeline->resize(w, h);
        ui_pipeline->resize(w, h);
        debug_control_pipeline->resize(w, h);
        hdr_renderer->resize(w, h);
        bloom_renderer->resize(w, h);
        blur_renderer->resize(w, h);
    }

    void App::key_press(int key) {
        print("key_press(): " << key);

        if (key == KEY::Esc)
            Window::close();

        if (key == KEY::N)
            enable_normal_mapping = !enable_normal_mapping;

        if (key == KEY::B)
            enable_blur = !enable_blur;

        if (key == KEY::H)
            enable_hdr = !enable_hdr;

        if (key == KEY::O)
            enable_ssao = !enable_ssao;

        if (key == KEY::F) {
            Window::toggle_window_mode();
        }

        if (key == KEY::E) {
            rock_sphere.add_component<Outline>();
        }

        if (key == KEY::R) {
            rock_sphere.remove_component<Outline>();
        }

        if (key == KEY::C) {
            camera.enable_look = !camera.enable_look;
        }
    }

    void App::key_release(int key) {
        print("key_release()");
    }

    void App::mouse_press(int button) {
        print("mouse_press()");
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            Cursor mouse_cursor = Window::mouse_cursor();
            static PBR_Pixel pixel;
            pbr_pipeline->read_pixel(pixel, (int) mouse_cursor.x, (int) mouse_cursor.y);
            print("Read pixel from [x,y] = [" << mouse_cursor.x << "," << mouse_cursor.y << "]");
            entity_control->select(mouse_cursor.x, mouse_cursor.y);
        }
    }

    void App::mouse_release(int button) {
        print("mouse_release()");
    }

    void App::mouse_cursor(double x, double y) {
        camera.look(x, y);
        entity_control->drag(x, y);
    }

    void App::mouse_scroll(double x, double y) {
        camera.zoom(x, y);
    }

    void App::entity_selected(ecs::Entity entity, double x, double y) {
        print("entity_selected: [" << x << "," << y << "]");
        auto& selected = entity.get_component<Selectable>()->enable;
        selected = !selected;
        if (selected) {
            entity.add_component<PolygonVisual>();
            entity.add_component<NormalVisual>();
        } else {
            entity.remove_component<PolygonVisual>();
            entity.remove_component<NormalVisual>();
        }
    }

    void App::entity_dragged(ecs::Entity entity, double x, double y) {
        print("entity_dragged: [" << x << "," << y << "]");
    }

    void App::entity_hovered(ecs::Entity entity, double x, double y) {
        print("entity_hovered: [" << x << "," << y << "]");
        entity.get_component<Material>()->color = { 5, 5, 5, 1 };
    }

    void App::render_screen_ui() {
        ui::theme_selector("Theme");
        ui::checkbox("HDR", &enable_hdr);
        ui::slider("Gamma", &screen_renderer->get_params().gamma.value, 1.2, 3.2, 0.1);
        ui::slider("Exposure", &hdr_renderer->get_params().exposure.value, 0, 5.0, 0.01);
        ui::checkbox("Normal Mapping", &enable_normal_mapping);
        ui::checkbox("Parallax Mapping", &enable_parallax_mapping);
        ui::checkbox("Blur", &enable_blur);
        ui::checkbox("BloomRenderer", &enable_bloom);
        ui::checkbox("SsaoRenderer", &enable_ssao);

        ui::slider("Sunlight X", &pbr_pipeline->sunlight.direction.x, -100, 100, 1);
        ui::slider("Sunlight Y", &pbr_pipeline->sunlight.direction.y, -100, 100, 1);
        ui::slider("Sunlight Z", &pbr_pipeline->sunlight.direction.z, -100, 100, 1);
        ui::color_picker("Sunlight Color", pbr_pipeline->sunlight.color);

        ui::slider("PointLight_1 X", &pbr_pipeline->point_lights[0].position.x, -25, 25, 1);
        ui::slider("PointLight_1 Y", &pbr_pipeline->point_lights[0].position.y, -25, 25, 1);
        ui::slider("PointLight_1 Z", &pbr_pipeline->point_lights[0].position.z, -25, 25, 1);
        ui::color_picker("PointLight_1 Color", pbr_pipeline->point_lights[0].color);

        ui::slider("PointLight_2 X", &pbr_pipeline->point_lights[1].position.x, -25, 25, 1);
        ui::slider("PointLight_2 Y", &pbr_pipeline->point_lights[1].position.y, -25, 25, 1);
        ui::slider("PointLight_2 Z", &pbr_pipeline->point_lights[1].position.z, -25, 25, 1);
        ui::color_picker("PointLight_2 Color", pbr_pipeline->point_lights[1].color);

        ui::slider("PointLight_3 X", &pbr_pipeline->point_lights[2].position.x, -25, 25, 1);
        ui::slider("PointLight_3 Y", &pbr_pipeline->point_lights[2].position.y, -25, 25, 1);
        ui::slider("PointLight_3 Z", &pbr_pipeline->point_lights[2].position.z, -25, 25, 1);
        ui::color_picker("PointLight_3 Color", pbr_pipeline->point_lights[2].color);

        ui::slider("PointLight_4 X", &pbr_pipeline->point_lights[3].position.x, -25, 25, 1);
        ui::slider("PointLight_4 Y", &pbr_pipeline->point_lights[3].position.y, -25, 25, 1);
        ui::slider("PointLight_4 Z", &pbr_pipeline->point_lights[3].position.z, -25, 25, 1);
        ui::color_picker("PointLight_4 Color", pbr_pipeline->point_lights[3].color);
    }

    void App::render_ui() {
        ui::begin();
        ui::window("Screen", [this]() { render_screen_ui(); });
        ui::end();
    }

    void App::render_postfx() {
        // Bloom effect
        if (enable_bloom) {
            bloom_renderer->get_hdr_buffer() = screen_renderer->get_params().scene_buffer;
            bloom_renderer->render();
            screen_renderer->get_params().scene_buffer = bloom_renderer->get_render_target();
        }
        // HDR effect
        if (enable_hdr) {
            hdr_renderer->get_params().scene_buffer = screen_renderer->get_params().scene_buffer;
            hdr_renderer->render();
            screen_renderer->get_params().scene_buffer = hdr_renderer->get_render_target();
        }
        // Blur effect
        if (enable_blur) {
            blur_renderer->get_params().scene_buffer = screen_renderer->get_params().scene_buffer;
            blur_renderer->render();
            screen_renderer->get_params().scene_buffer = blur_renderer->get_render_target();
        }
    }

    void App::render_debug_screen() {
        if (Window::is_key_press(KEY::D1)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_render_target();
        }

        else if (Window::is_key_press(KEY::D2)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_gbuffer().position;
        }

        else if (Window::is_key_press(KEY::D3)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_gbuffer().normal;
        }

        else if (Window::is_key_press(KEY::D4)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_gbuffer().albedo;
        }

        else if (Window::is_key_press(KEY::D5)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_gbuffer().pbr_params;
        }

        else if (Window::is_key_press(KEY::D6)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_gbuffer().shadow_proj_coords;
        }

        else if (Window::is_key_press(KEY::D8)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->env.hdr;
        }

        else if (Window::is_key_press(KEY::D9)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_transparent_buffer().revealage;
        }

        else if (Window::is_key_press(KEY::D0)) {
            screen_renderer->get_params().ui_buffer = ui_pipeline->get_render_target();
        }

        else if (Window::is_key_press(KEY::P)) {
            screen_renderer->get_params().debug_control_buffer = debug_control_pipeline->get_render_target();
        }

        else if (Window::is_key_press(KEY::T)) {
            screen_renderer->get_params().scene_buffer = font_roboto_regular->buffer;
        }
    }

    void App::render() {
        pbr_pipeline->render();
        screen_renderer->get_params().scene_buffer = pbr_pipeline->get_render_target();

        ui_pipeline->render();
        screen_renderer->get_params().ui_buffer = ui_pipeline->get_render_target();

#ifdef DEBUG
        debug_control_pipeline->render();
        screen_renderer->get_params().debug_control_buffer = debug_control_pipeline->get_render_target();
#endif

        render_postfx();

        render_debug_screen();

        screen_renderer->render();
    }

    void App::init_text() {
        FontAtlas::init();

        font_roboto_regular = FontAtlas::load("fonts/Roboto-Regular.ttf", 40);
        font_roboto_regular->save_bmp("fonts/Roboto-Regular.bmp");
        font_roboto_regular->save_widths("fonts/Roboto-Regular.widths");

        text_label = &scene;
        text_label.add_component<Text3d>(font_roboto_regular, "Hello World!");
        text_label.get_component<Text3d>()->transform.translation = { 0, 5, 4 };
    }
}