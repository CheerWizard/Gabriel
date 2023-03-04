#include <app.h>

#include <window.h>
#include <ui.h>

#include <camera.h>
#include <light.h>
#include <model_loader.h>
#include <outline.h>
#include <shadow.h>
#include <math_functions.h>
#include <sphere.h>

#include <map>
#include <random>

namespace gl {

    static bool running = true;
    static float dt = 6;
    static float begin_time = 0;

    static bool enable_fullscreen = false;

    static Shader material_shader;
    static UniformBuffer sunlight_ubo;
    static UniformBuffer lights_ubo;
    static UniformBuffer flashlight_ubo;

    static Camera camera;
    static bool enable_camera = true;

    static DirectLight sunlight;

    static LightPresent point_light_present;
    static std::array<PointLight, 4> point_lights;

    static SpotLight flashlight;

    static DrawableElements plane;
    static Transform plane_transform = {
            { 0, -2, 4 },
            { 0, 0, 0 },
            { 40, 0.25, 40 }
    };
    static Material plane_material;

    static DrawableElements sphere;
    static DrawableElements sphere_rock;
    static SphereTBN sphere_rock_geometry;
    static std::vector<Transform> sphere_transforms = {
            {
                    { 0, 0, 0 },
                    { 0, 0, 0 },
                    { 1, 1, 1 }
            },
            {
                    { 0, 0, 4 },
                    { 0, 0, 0 },
                    { 1, 1, 1 }
            },
            {
                    { 0, 0, 8 },
                    { 0, 0, 0 },
                    { 1, 1, 1 }
            }
    };
    static std::vector<Material> sphere_materials;
    static DrawableElements sphere_shadow;
    static DrawableElements sphere_rock_shadow;
    static SphereDefault sphere_rock_shadow_geometry;

    static io::DrawableModel model;
    static Transform model_transform = {
            { -3, -0.1, 5 },
            { 0, 0, 0 },
            { 1, 1, 1 }
    };
    static Material model_material;
    static io::ShadowDrawableModel model_shadow;

    static std::map<float, Transform> transparent_objects;

    static Shader screen_shader;
    static VertexArray screen_vao;
    static float screen_gamma = 2.2f;
    static float screen_exposure = 1.0f;
    static ColorAttachment final_render_target;

    static FrameBuffer scene_fbo;

    static FrameBuffer msaa_fbo;
    static int msaa = 8;

    static FrameBuffer blur_fbo;
    static Shader blur_shader;
    static float blur_offset = 1.0 / 300.0;
    static int enable_blur = false;

    static FrameBuffer ssao_fbo;
    static Shader ssao_shader;
    static std::vector<glm::vec3> ssao_kernel;
    static std::vector<glm::vec3> ssao_noise;
    static Texture ssao_noise_texture;
    static int enable_ssao = false;

    static EnvLight env_light;
    static Texture env_hdr_texture;

    static DirectShadow direct_shadow;
    static PointShadow point_shadow;

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
        camera.resize(w, h);
        scene_fbo.resize(w, h);
        msaa_fbo.resize(w, h);
        blur_fbo.resize(w, h);
        ssao_fbo.resize(w, h);
    }

    static int enable_normal_mapping = true;
    static int enable_parallax_mapping = true;

    static void key_press(int key) {
        print("key_press(): " << key);

        if (key == KEY::Esc)
            win::close();

        if (key == KEY::N) {
            enable_normal_mapping = !enable_normal_mapping;
        }

        if (key == KEY::P) {
            enable_parallax_mapping = !enable_parallax_mapping;
        }

        if (key == KEY::B)
            enable_blur = !enable_blur;

        if (key == KEY::O)
            enable_ssao = !enable_ssao;

        if (key == KEY::C)
            enable_camera = !enable_camera;

        if (key == KEY::F) {
            enable_fullscreen = !enable_fullscreen;
            if (enable_fullscreen)
                win::set_full_screen();
            else
                win::set_windowed();
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
        if (enable_camera) {
            camera.look(x, y);
        }
    }

    static void mouse_scroll(double x, double y) {
        if (enable_camera) {
            camera.zoom(x, y);
        }
    }

    static void init() {
        win::init({ 0, 0, 800, 600, "CGP", win::win_flags::sync });

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

        win::event_registry_update();

        // setup shaders
        material_shader.init(
            "shaders/material.vert",
            "shaders/material.frag"
        );
        screen_shader.init(
            "shaders/screen.vert",
            "shaders/screen.frag"
        );
        blur_shader.init(
            "shaders/screen.vert",
            "shaders/blur.frag"
        );

        // setup main camera
        camera.init(0, win::get_aspect_ratio());
        camera.max_pitch = 180;

        // setup screen
        screen_vao.init();

        // setup scene frame
        ColorAttachment scene_color = { win::props().width, win::props().height };
        scene_color.data.internal_format = GL_RGBA16F;
        scene_color.data.data_format = GL_RGBA;
        scene_color.data.primitive_type = GL_FLOAT;
        ColorAttachment scene_bright_color = scene_color;
        scene_fbo.colors.emplace_back(scene_color); // main
        scene_fbo.colors.emplace_back(scene_bright_color); // brightness
        scene_fbo.rbo = { win::props().width, win::props().height };
        scene_fbo.flags |= gl::init_render_buffer;
        scene_fbo.init();

        // setup MSAA frame
        ColorAttachment msaa_color = { win::props().width, win::props().height, msaa };
        msaa_color.data.internal_format = GL_RGBA16F;
        msaa_color.data.data_format = GL_RGBA;
        msaa_color.data.primitive_type = GL_FLOAT;
        msaa_color.view.type = GL_TEXTURE_2D_MULTISAMPLE;
        ColorAttachment msaa_bright_color = msaa_color;
        msaa_fbo.colors.emplace_back(msaa_color); // main
        msaa_fbo.colors.emplace_back(msaa_bright_color); // brightness
        msaa_fbo.rbo = { win::props().width, win::props().height, msaa };
        msaa_fbo.flags |= gl::init_render_buffer;
        msaa_fbo.init();

        // setup Blur frame
        ColorAttachment blur_color = { win::props().width, win::props().height };
        blur_color.data.internal_format = GL_RGBA16F;
        blur_color.data.data_format = GL_RGBA;
        blur_color.data.primitive_type = GL_FLOAT;
        blur_fbo.colors.emplace_back(blur_color);
        blur_fbo.init();

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
        TextureParams ssao_noise_params;
        ssao_noise_params.min_filter = GL_NEAREST;
        ssao_noise_params.mag_filter = GL_NEAREST;
        ssao_noise_texture.init(
                { 4, 4, GL_RGB16F, GL_RGB, GL_FLOAT, &ssao_noise[0] },
                ssao_noise_params
        );
        // SSAO frame
        ColorAttachment ssao_color = { win::props().width, win::props().height };
        ssao_color.data.internal_format = GL_RED;
        ssao_color.data.data_format = GL_RED;
        ssao_color.data.primitive_type = GL_FLOAT;
        ssao_color.params.min_filter = GL_NEAREST;
        ssao_color.params.mag_filter = GL_NEAREST;
        ssao_fbo.colors.emplace_back(ssao_color);
        ssao_fbo.init();

        // setup uniform buffers
        sunlight_ubo.init(1, sizeof(sunlight));
        lights_ubo.init(2, sizeof(point_lights));
        flashlight_ubo.init(3, sizeof(flashlight));

        // setup sunlight
        static const float sunlight_intensity = 0.03;
        static const glm::vec3 sunlight_rgb = glm::vec3(237, 213, 158) * sunlight_intensity;
        sunlight.color = { sunlight_rgb, 1 };
        sunlight.direction = { -1, -1, -1, 0 };
        sunlight_ubo.update({ 0, sizeof(sunlight), &sunlight });

        // setup lights
        point_lights[0].position = { -4, -1, 0, 1 };
        point_lights[0].color = { 1, 0, 0, 1 };
        point_lights[0].constant = 0;
        point_lights[0].linear = 0;
        point_lights[0].quadratic = 1;
        point_lights[1].position = { 4, -1, 0, 1 };
        point_lights[1].color = { 0, 1, 0, 1 };
        point_lights[1].constant = 0;
        point_lights[1].linear = 0;
        point_lights[1].quadratic = 1;
        point_lights[2].position = { -4, -1, 8, 1 };
        point_lights[2].color = { 0, 0, 1, 1 };
        point_lights[2].constant = 0;
        point_lights[2].linear = 0;
        point_lights[2].quadratic = 1;
        point_lights[3].position = { 4, -1, 8, 1 };
        point_lights[3].color = { 1, 0, 1, 1 };
        point_lights[3].constant = 0;
        point_lights[3].linear = 0;
        point_lights[3].quadratic = 1;
        lights_ubo.update({ 0, sizeof(point_lights), point_lights.data() });

        // setup flashlight
        flashlight.position = { camera.position, 0 };
        flashlight.direction = { camera.front, 0 };
        flashlight.color = { 0, 0, 0, 1 };
        flashlight_ubo.update({ 0, sizeof(flashlight), &flashlight });

        // setup env light
        env_light.init();
        env_hdr_texture.init_hdr("images/hdr/Arches_E_PineTree_3k.hdr", true);
        env_light.generate(env_hdr_texture);
        env_light.update(material_shader);

        // setup light presentation
        point_light_present.color = { 0, 0, 1, 0.5 };
        point_light_present.init();

        // setup shadow
        direct_shadow.width = 1024;
        direct_shadow.height = 1024;
        direct_shadow.direction = sunlight.direction;
        direct_shadow.init();

        point_shadow.width = 1024;
        point_shadow.height = 1024;
        point_shadow.position = { 0, 0, 0 };
        point_shadow.init();

        // setup 3D model
        model.init("models/backpack/backpack.obj");
        model_shadow.init(model);
        model_material.init(
                true,
                "models/backpack/diffuse.jpg",
                "models/backpack/normal.png",
                null,
                "models/backpack/specular.jpg",
                "models/backpack/roughness.jpg",
                "models/backpack/ao.jpg"
        );
        model_material.metallic_factor = 1;
        model_material.roughness_factor = 1;
        model_material.ao_factor = 1;

        // setup sphere
        SphereTBN sphere_geometry;
        sphere_geometry.init_tbn(sphere);
        SphereTBN sphere_shadow_geometry;
        sphere_shadow_geometry.init_default(sphere_shadow);
        // setup rock sphere
        sphere_rock_geometry.x_segments = 2047;
        sphere_rock_geometry.y_segments = 2047;
        sphere_rock_geometry.init_tbn(sphere_rock);
        sphere_rock_shadow_geometry.x_segments = 2047;
        sphere_rock_shadow_geometry.y_segments = 2047;
        sphere_rock_shadow_geometry.init_default(sphere_rock_shadow);

        sphere_materials.resize(sphere_transforms.size());
        {
            sphere_materials[0].init(
                    false,
                    "images/bumpy-rockface1-bl/albedo.png",
                    "images/bumpy-rockface1-bl/normal.png",
                    null,
                    "images/bumpy-rockface1-bl/metallic.png",
                    "images/bumpy-rockface1-bl/roughness.png",
                    "images/bumpy-rockface1-bl/ao.png"
            );
            sphere_materials[0].metallic_factor = 1;
            sphere_materials[0].roughness_factor = 1;
            sphere_materials[0].ao_factor = 1;

            sphere_rock_geometry.displace(sphere_rock, "images/bumpy-rockface1-bl/height.png", false, 3.0f);
            sphere_rock_shadow_geometry.displace(sphere_rock_shadow, "images/bumpy-rockface1-bl/height.png", false, 3.0f, 0, [](gl::VertexDefault& V) {});

            sphere_materials[1].init(
                    false,
                    "images/cheap-plywood1-bl/albedo.png",
                    "images/cheap-plywood1-bl/normal.png",
                    null,
                    "images/cheap-plywood1-bl/metallic.png",
                    "images/cheap-plywood1-bl/roughness.png",
                    "images/cheap-plywood1-bl/ao.png"
            );
            sphere_materials[1].metallic_factor = 1;
            sphere_materials[1].roughness_factor = 1;
            sphere_materials[1].ao_factor = 1;

            sphere_materials[2].init(
                    false,
                    "images/light-gold-bl/albedo.png",
                    "images/light-gold-bl/normal.png",
                    null,
                    "images/light-gold-bl/metallic.png",
                    "images/light-gold-bl/roughness.png",
                    null
            );
            sphere_materials[2].metallic_factor = 1;
            sphere_materials[2].roughness_factor = 1;
            sphere_materials[2].ao_factor = 1;
        }

        // setup horizontal plane
        CubeTBN plane_geometry;
        plane_geometry.init_tbn(plane);
        plane_material.init(
            false,
            null,
            null
        );
        plane_material.color = { 1, 1, 1, 0.5 };
        plane_material.metallic_factor = 0;
        plane_material.roughness_factor = 0.05;
        plane_material.ao_factor = 1;

        blur_shader.use();
        blur_shader.set_uniform_args("offset", blur_offset);

        Texture::unbind();
        Shader::stop();
    }

    static void free() {
        sphere.free();
        sphere_shadow.free();
        Material::free(sphere_materials);
        sphere_rock.free();
        sphere_rock_shadow.free();

        screen_shader.free();
        screen_vao.free();

        blur_shader.free();
        blur_fbo.free();

        ssao_shader.free();
        ssao_fbo.free();
        ssao_noise_texture.free();

        direct_shadow.free();
        point_shadow.free();

        msaa_fbo.free();

        scene_fbo.free();

        plane.free();
        plane_material.free();

        material_shader.free();

        sunlight_ubo.free();
        lights_ubo.free();
        flashlight_ubo.free();
        env_light.free();
        env_hdr_texture.free();

        point_light_present.free();

        model.free();
        model_shadow.free();

        camera.free();

        win::free();
    }

    static void camera_control_update() {
        if (!enable_camera)
            return;

        float camera_speed = camera.speed / dt;
        glm::vec3& camera_pos = camera.position;

        if (win::is_key_press(KEY::W)) {
            camera_pos += camera_speed * camera.front;
        }
        else if (win::is_key_press(KEY::A)) {
            camera_pos -= glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;
        }
        else if (win::is_key_press(KEY::S)) {
            camera_pos -= camera_speed * camera.front;
        }
        else if (win::is_key_press(KEY::D)) {
            camera_pos += glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;
        }

        camera.update_view();
        material_shader.use();
        material_shader.set_uniform_args("view_position", camera.position);
    }

    static void simulate() {
        float t = begin_time;
        camera_control_update();
        // bind flashlight to camera
        flashlight.position = { camera.position, 0 };
        flashlight.direction = { camera.front, 0 };
        // rotate object each frame
        float f = 0.05f;
        sphere_transforms[0].rotation.y += f;
        sphere_transforms[1].rotation.y += f * 2;
        sphere_transforms[2].rotation.y += f * 4;
        // translate point lights up/down
        for (auto& point_light : point_lights) {
            point_light.position.y = 5 * sin(t/5) + 2;
        }
        // sorting transparent drawables
        transparent_objects.clear();
    }

    static void render_screen_ui() {
        ui::theme_selector("Theme");
        ui::slider("Gamma", &screen_gamma, 1.2, 3.2, 0.1);
        ui::slider("Exposure", &screen_exposure, 0, 5.0, 0.01);
        ui::checkbox("Normal Mapping", &enable_normal_mapping);
        ui::checkbox("Parallax Mapping", &enable_parallax_mapping);
        ui::checkbox("Blur", &enable_blur);
        ui::checkbox("SSAO", &enable_ssao);

        ui::slider("Sunlight X", &sunlight.direction.x, -100, 100, 1);
        ui::slider("Sunlight Y", &sunlight.direction.y, -100, 100, 1);
        ui::slider("Sunlight Z", &sunlight.direction.z, -100, 100, 1);
        ui::color_picker("Sunlight Color", sunlight.color);

        ui::slider("PointLight_1 X", &point_lights[0].position.x, -25, 25, 1);
        ui::slider("PointLight_1 Y", &point_lights[0].position.y, -25, 25, 1);
        ui::slider("PointLight_1 Z", &point_lights[0].position.z, -25, 25, 1);
        ui::color_picker("PointLight_1 Color", point_lights[0].color);

        ui::slider("PointLight_2 X", &point_lights[1].position.x, -25, 25, 1);
        ui::slider("PointLight_2 Y", &point_lights[1].position.y, -25, 25, 1);
        ui::slider("PointLight_2 Z", &point_lights[1].position.z, -25, 25, 1);
        ui::color_picker("PointLight_2 Color", point_lights[1].color);

        ui::slider("PointLight_3 X", &point_lights[2].position.x, -25, 25, 1);
        ui::slider("PointLight_3 Y", &point_lights[2].position.y, -25, 25, 1);
        ui::slider("PointLight_3 Z", &point_lights[2].position.z, -25, 25, 1);
        ui::color_picker("PointLight_3 Color", point_lights[2].color);

        ui::slider("PointLight_4 X", &point_lights[3].position.x, -25, 25, 1);
        ui::slider("PointLight_4 Y", &point_lights[3].position.y, -25, 25, 1);
        ui::slider("PointLight_4 Z", &point_lights[3].position.z, -25, 25, 1);
        ui::color_picker("PointLight_4 Color", point_lights[3].color);
    }

    static void render_ui() {
        ui::begin();
        ui::window("Screen", render_screen_ui);
        ui::end();
    }

    static void render_scene() {
//        // render Shadow pass
//        {
//            // render direct shadow
//            gl::direct_shadow_begin();
//            {
//                gl::direct_shadow_update(sunlight.direction);
//                gl::direct_shadow_draw(model_transform, model_shadow.elements);
//                for (auto& sphere_transform : sphere_transforms) {
//                    gl::direct_shadow_draw(sphere_transform, sphere_shadow);
//                }
//            }
//            // render point shadow
//            gl::point_shadow_begin();
//            {
//                for (auto& point_light : point_lights) {
//                    gl::point_shadow_update(point_light.position);
//                    gl::point_shadow_draw(model_transform, model_shadow.elements);
//                    for (auto& sphere_transform : sphere_transforms) {
//                        gl::point_shadow_draw(sphere_transform, sphere_shadow);
//                    }
//                }
//            }
//        }
//        gl::shadow_end();

        // render MSAA or Scene pass
        if (msaa > 1) {
            msaa_fbo.bind();
        } else {
            scene_fbo.bind();
        }
        glViewport(0, 0, win::props().width, win::props().height);
        gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glStencilMask(0x00);

        // render HDR env
        env_light.render();

        // upload shadow maps
        {
//            gl::shader_use(material_shader);
//            gl::direct_shadow_update(material_shader);
//            gl::point_shadow_update(material_shader);
        }

        // render and upload lights

        // sunlight
        {
            sunlight_ubo.update({ 0, sizeof(sunlight), &sunlight });
        }
        // point lights
        {
            lights_ubo.update({ 0, sizeof(point_lights), point_lights.data() });
            // update light presentation
            for (auto& point_light : point_lights) {
                point_light_present.transform.translation = point_light.position;
                point_light_present.update();
            }
        }
        // flashlight
        {
            flashlight_ubo.update({ 0, sizeof(flashlight), &flashlight });
        }

        // render material objects
        {
            material_shader.use();

            plane_material.update(material_shader);
            plane_transform.update(material_shader);
            plane.draw();

            sphere_materials[0].update(material_shader);
            sphere_transforms[0].update(material_shader);
            sphere_rock.draw();

            for (int i = 1 ; i < sphere_transforms.size() ; i++) {
                sphere_materials[i].update(material_shader);
                sphere_transforms[i].update(material_shader);
                sphere.draw();
            }

            model_material.update(material_shader);
            model_transform.update(material_shader);
            model.elements.draw();
        }

        // render outlined objects
        {
            // render default state
//            gl::outline_end();
//            gl::shader_use(material_shader);
//            gl::transform_update(material_shader, model_transform);
//            gl::material_update(material_shader, model_material);
//            gl::draw(model.elements);
            // render outline state
//            gl::outline_begin();
//            gl::outline_draw(model_transform, model.elements, model_outline);
//            gl::outline_end();
        }

        // render transparent objects
        {
            material_shader.use();
            for (auto it = transparent_objects.rbegin() ; it != transparent_objects.rend() ; it++) {
                it->second.update(material_shader);
            }
        }

        // MSAA -> scene pass
        if (msaa > 1) {
            int w = win::props().width;
            int h = win::props().height;
            FrameBuffer::blit(msaa_fbo.id, w, h, scene_fbo.id, w, h, msaa_fbo.colors.size());
        }

        // scene -> post effects
        final_render_target = scene_fbo.colors[0];

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

        // post effects
        {
            // blur effect
            if (enable_blur) {
                blur_fbo.bind();
                clear_display(1, 1, 1, 1, GL_COLOR_BUFFER_BIT);
                blur_shader.use();
                final_render_target.view.update(blur_shader);
                screen_vao.draw_quad();
                final_render_target = blur_fbo.colors[0];
            }
        }

        // render into screen
        {
            FrameBuffer::unbind();
            clear_display(1, 1, 1, 1, GL_COLOR_BUFFER_BIT);
            screen_shader.use();
            final_render_target.view.update(screen_shader);
            screen_shader.set_uniform_args("gamma", screen_gamma);
            screen_shader.set_uniform_args("exposure", screen_exposure);
            screen_vao.draw_quad();
        }
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

            render_scene();

#ifdef UI
            render_ui();
#endif

            win::swap();

            dt = ((float)glfwGetTime() - begin_time) * 1000;

            print_dt();
        }
    }
}