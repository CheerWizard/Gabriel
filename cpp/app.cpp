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

            Window::swap();

            delta_time = ((float)glfwGetTime() - begin_time) * 1000;

            print_dt();
        }
    }

    void App::init_window() {
        Window::init({ 0, 0, 800, 600, "Educational Project", WindowFlags::Sync });

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
                { 0, 0, 4 },
                { 0, 0, 0 },
                { 1, 1, 1 },
                16
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

        terrain.add_component<PBR_Component_Deferred>();
        terrain.add_component<Selectable>();
        terrain.add_component<Draggable>();
        terrain.init();

        rock_sphere.add_component<PBR_Component_DeferredCull>();
        rock_sphere.add_component<Selectable>(entity_selected);
        rock_sphere.add_component<Draggable>(entity_dragged);
        rock_sphere.add_component<SphereCollider>(rock_sphere.transform()->translation, 3.0f);
        rock_sphere.add_component<Shadowable>();

        wood_sphere.add_component<PBR_Component_DeferredCull>();
        wood_sphere.add_component<Selectable>(entity_selected);
        wood_sphere.add_component<Draggable>(entity_dragged);
        wood_sphere.add_component<Shadowable>();

        metal_sphere.add_component<PBR_Component_DeferredCull>();
        metal_sphere.add_component<Selectable>(entity_selected);
        metal_sphere.add_component<Draggable>(entity_dragged);
        metal_sphere.add_component<Shadowable>();

        backpack.add_component<PBR_Component_DeferredCull>();
        backpack.add_component<Selectable>(entity_selected);
        backpack.add_component<Draggable>(entity_dragged);
        backpack.add_component<Shadowable>();

        human.add_component<PBR_SkeletalComponent_DeferredCull>();
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
        backpack.material()->roughness_factor = 0.5;
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

                terrain.displace_with(MidPointFormation(terrain.size(), terrain.size(), 10, 0, 1), 1);
                image_mixer.displacement_map = &terrain.displacement()->map;
                image_mixer.add_image({ -1.0, -0.5, 0 }, "images/terrain/rock_tile.png");
                image_mixer.add_image({ 0, 0.1, 0.2 }, "images/terrain/sand_tile.jpg");
                image_mixer.add_image({ 0.20, 0.45, 0.70 }, "images/terrain/grass_tile.png");
                image_mixer.add_image({ 0.70, 0.77, 0.85 }, "images/terrain/rock_tile.png");
                image_mixer.add_image({ 0.85, 0.90, 1.0 }, "images/terrain/snow_tile.png");
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
    }

    void App::init_pipeline() {
        screen_renderer = new ScreenRenderer();

        ui_pipeline = new UI_Pipeline(&scene);

        debug_control_pipeline = new DebugControlPipeline(&scene);

        mShadowPipeline = new ShadowPipeline(&scene, Window::get_width(), Window::get_height());

        pbr_pipeline = new PBR_Pipeline(&scene, Window::get_width(), Window::get_height());
        pbr_pipeline->setDirectShadow(&mShadowPipeline->directShadow);
        pbr_pipeline->setPointShadow(&mShadowPipeline->pointShadow);

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
        pbr_pipeline->env.resolution = { 1024, 1024 };
        pbr_pipeline->env.prefilter_resolution = { 512, 512 };
        pbr_pipeline->env.init();
        pbr_pipeline->init_hdr_env("images/hdr/Arches_E_PineTree_3k.hdr", true);
        pbr_pipeline->generate_env();
        // setup sunlight
        sunlight = &scene;
        sunlight.value().color = { 244 * 0.1f, 233 * 0.1f, 155 * 0.1f, 1 };
        sunlight.value().direction = { 5, 5, 5, 0 };
        // setup point lights
        for (auto& pointLight : pointLights) {
            pointLight = &scene;
        }
        pointLights[0].value().position = { -4, 2, 0, 1 };
        pointLights[0].value().color = glm::vec4 { 0, 0, 0, 1 };
        pointLights[1].value().position = { 4, 3, 0, 1 };
        pointLights[1].value().color = glm::vec4 { 0, 0, 0, 1 };
        pointLights[2].value().position = { -4, 4, 8, 1 };
        pointLights[2].value().color = glm::vec4 { 0, 0, 0, 1 };
        pointLights[3].value().position = { 4, 5, 8, 1 };
        pointLights[3].value().color = glm::vec4 { 0, 0, 0, 1 };
        // setup flashlight
        flashlight = &scene;
        flashlight.value().position = { camera.position, 0 };
        flashlight.value().direction = { camera.front, 0 };
        flashlight.value().color = { 0, 0, 0, 0 };
        // update light buffers
        pbr_pipeline->updateSunlight(sunlight.value());
        std::array<PointLightUniform, 4> pointLightUniforms = {
                pointLights[0].value(),
                pointLights[1].value(),
                pointLights[2].value(),
                pointLights[3].value()
        };
        pbr_pipeline->updatePointLights(pointLightUniforms);
        pbr_pipeline->updateFlashlight(flashlight.value());
    }

    void App::free() {
        FontAtlas::free();

        terrain.free();

        delete screen_renderer;

        delete pbr_pipeline;
        delete ui_pipeline;
        delete debug_control_pipeline;
        delete mShadowPipeline;

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
        flashlight.value().position = { camera.position, 0 };
        flashlight.value().direction = { camera.front, 0 };

        // rotate object each frame
        float f = 0.05f;
        rock_sphere.transform()->rotation.y += f;
        wood_sphere.transform()->rotation.y += f * 2;
        metal_sphere.transform()->rotation.y += f * 4;
        human.transform()->rotation.y += f * 4;

        // translate sunlight
        float sunlight_translate = 5 * sin(t/5) + 5;
        sunlight.value().direction.x = sunlight_translate;
        sunlight.value().direction.z = sunlight_translate;
        pbr_pipeline->updateSunlight(sunlight.value());

        // translate point lights up/down
        std::array<PointLightUniform, 4> pointLightUniforms;
        for (int i = 0 ; i < 4 ; i++) {
            auto& pointLight = pointLights[i];
            pointLight.value().position.y = sin(t/5);
            pointLightUniforms[i] = pointLight.value();
        }
        pbr_pipeline->updatePointLights(pointLightUniforms);

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
        mShadowPipeline->resize(w, h);
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
            screen_renderer->get_params().scene_buffer = mShadowPipeline->directShadow.map.buffer;
        }

        else if (Window::is_key_press(KEY::D7)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->get_ssao_buffer();
        }

        else if (Window::is_key_press(KEY::D8)) {
            screen_renderer->get_params().scene_buffer = pbr_pipeline->env.brdf_convolution;
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
        mShadowPipeline->render();

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