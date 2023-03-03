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

namespace app {

    static bool running = true;
    static float dt = 6;
    static float begin_time = 0;

    static u32 material_shader;
    static u32 sunlight_ubo;
    static u32 lights_ubo;
    static u32 flashlight_ubo;

    static gl::camera camera;
    static bool enable_camera = false;

    static gl::light_dir sunlight;

    static gl::light_present point_light_present;
    static std::array<gl::light_point, 4> point_lights;

    static gl::light_spot flashlight;

    static io::drawable_elements plane;
    static gl::transform plane_transform = {
            { 0, -2, 4 },
            { 0, 0, 0 },
            { 1, 0.5, 1 }
    };
    static gl::material plane_material;

    static io::drawable_elements sphere;
    static std::vector<gl::transform> sphere_transforms = {
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
    static std::vector<gl::material> sphere_materials;
    static io::drawable_elements sphere_shadow;

    static io::drawable_model model;
    static gl::transform model_transform = {
            { 5, 0, 5 },
            { 0, 90, 0 },
            { 1, 1, 1 }
    };
    static gl::material model_material;
    static io::shadow_drawable_model model_shadow;

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

    static u32 hdr_env_fbo;
    static gl::render_buffer hdr_env_rbo;
    static int env_resolution = 512;

    static u32 hdr_to_cubemap_shader;
    static gl::texture env_hdr_texture;

    static u32 hdr_irradiance_shader;
    static gl::texture env_irradiance_map;

    static u32 env_shader;
    static gl::drawable_elements env_cube;
    static gl::texture env_cube_map;

    static u32 hdr_prefilter_convolution_shader;
    static gl::texture env_prefilter_map;
    static int env_prefilter_mip_levels = 5;

    static u32 brdf_convolution_shader;
    static gl::texture env_brdf_convolution_map;

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
        gl::camera_resize(camera, w, h);
    }

    static int enable_normal_mapping = true;
    static int enable_parallax_mapping = true;
    static int enable_irradiance_mapping = true;
    static int enable_env_prefilter = true;
    static int enable_brdf_convolution = true;

    static void update_mappings_state() {
        for (auto& sphere_material : sphere_materials) {
            sphere_material.enable_env_irradiance = enable_irradiance_mapping;
            sphere_material.enable_env_prefilter = enable_env_prefilter;
            sphere_material.enable_brdf_convolution = enable_brdf_convolution;
        }

        plane_material.enable_env_irradiance = enable_irradiance_mapping;
        plane_material.enable_env_prefilter = enable_env_prefilter;
        plane_material.enable_brdf_convolution = enable_brdf_convolution;

        model_material.enable_env_irradiance = enable_irradiance_mapping;
        model_material.enable_env_prefilter = enable_env_prefilter;
        model_material.enable_brdf_convolution = enable_brdf_convolution;
    }

    static void key_press(int key) {
        print("key_press(): " << key);

        if (key == KEY::Escape)
            win::close();

        if (key == KEY::N) {
            enable_normal_mapping = !enable_normal_mapping;
            update_mappings_state();
        }

        if (key == KEY::P) {
            enable_parallax_mapping = !enable_parallax_mapping;
            update_mappings_state();
        }

        if (key == KEY::I) {
            enable_irradiance_mapping = !enable_irradiance_mapping;
            update_mappings_state();
        }

        if (key == KEY::B)
            enable_blur = !enable_blur;

        if (key == KEY::O)
            enable_ssao = !enable_ssao;

        if (key == KEY::C)
            enable_camera = !enable_camera;
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
            gl::camera_look(camera, x, y);
        }
    }

    static void mouse_scroll(double x, double y) {
        if (enable_camera) {
            gl::camera_zoom(camera, x, y);
        }
    }

    static void init() {
        win::init({ 1920, 1000, "CGP", true });
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
        hdr_to_cubemap_shader = gl::shader_init({
            "shaders/hdr_to_cubemap.vert",
            "shaders/hdr_to_cubemap.frag"
        });
        hdr_irradiance_shader = gl::shader_init({
            "shaders/hdr_irradiance.vert",
            "shaders/hdr_irradiance.frag"
        });
        hdr_prefilter_convolution_shader = gl::shader_init({
            "shaders/hdr_prefilter_convolution.vert",
            "shaders/hdr_prefilter_convolution.frag"
        });
        brdf_convolution_shader = gl::shader_init({
            "shaders/brdf_convolution.vert",
            "shaders/brdf_convolution.frag"
        });
        env_shader = gl::shader_init({
            "shaders/cubemap.vert",
            "shaders/hdr_cubemap.frag"
        });

        // setup main camera
        camera = gl::camera_init(0);
        camera.max_pitch = 180;

        // setup screen
        screen_vao = gl::vao_init();

        // setup scene frame
        gl::color_attachment scene_color = { win::props().width, win::props().height };
        scene_color.data.internal_format = GL_RGBA16F;
        scene_color.data.data_format = GL_RGBA;
        scene_color.data.primitive_type = GL_FLOAT;
        gl::color_attachment scene_bright_color = scene_color;
        scene_colors.emplace_back(scene_color); // main
        scene_colors.emplace_back(scene_bright_color); // brightness
        scene_rbo = { win::props().width, win::props().height };
        scene_fbo = gl::fbo_init(&scene_colors, null, null, &scene_rbo);

        // setup MSAA frame
        gl::color_attachment msaa_color = { win::props().width, win::props().height, msaa };
        msaa_color.data.internal_format = GL_RGBA16F;
        msaa_color.data.data_format = GL_RGBA;
        msaa_color.data.primitive_type = GL_FLOAT;
        msaa_color.view.type = GL_TEXTURE_2D_MULTISAMPLE;
        gl::color_attachment msaa_bright_color = msaa_color;
        msaa_colors.emplace_back(msaa_color); // main
        msaa_colors.emplace_back(msaa_bright_color); // brightness
        msaa_rbo = { win::props().width, win::props().height, msaa };
        msaa_fbo = gl::fbo_init(&msaa_colors, null, null, &msaa_rbo);

        // setup Blur frame
        gl::color_attachment blur_color = { win::props().width, win::props().height };
        blur_color.data.internal_format = GL_RGBA16F;
        blur_color.data.data_format = GL_RGBA;
        blur_color.data.primitive_type = GL_FLOAT;
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
        gl::texture_params ssao_noise_params;
        ssao_noise_params.min_filter = GL_NEAREST;
        ssao_noise_params.mag_filter = GL_NEAREST;
        gl::texture_init(
                ssao_noise_texture,
                { 4, 4, GL_RGB16F, GL_RGB, GL_FLOAT, &ssao_noise[0] },
                ssao_noise_params
        );
        // SSAO frame
        gl::color_attachment ssao_color = { win::props().width, win::props().height };
        ssao_color.data.internal_format = GL_RED;
        ssao_color.data.data_format = GL_RED;
        ssao_color.data.primitive_type = GL_FLOAT;
        ssao_color.params.min_filter = GL_NEAREST;
        ssao_color.params.mag_filter = GL_NEAREST;
        ssao_colors.emplace_back(ssao_color);
        ssao_fbo = gl::fbo_init(&ssao_colors, null, null, null);

        // setup env
        hdr_env_rbo = {env_resolution, env_resolution };
        hdr_env_rbo.format = GL_DEPTH_COMPONENT24;
        hdr_env_rbo.type = GL_DEPTH_ATTACHMENT;
        hdr_env_fbo = gl::fbo_init(hdr_env_rbo);
        // create env maps
        gl::texture_hdr_init(env_hdr_texture, "images/hdr/Arches_E_PineTree_3k.hdr", true);
        gl::texture_params env_map_params;
        env_map_params.s = GL_CLAMP_TO_EDGE;
        env_map_params.t = GL_CLAMP_TO_EDGE;
        env_map_params.r = GL_CLAMP_TO_EDGE;
        env_map_params.min_filter = GL_LINEAR;
        env_map_params.mag_filter = GL_LINEAR;
        gl::texture_cube_init(
                env_irradiance_map,
                { 32, 32, GL_RGB16F, GL_RGB, GL_FLOAT },
                env_map_params
        );
        gl::texture_init(
                env_brdf_convolution_map,
                { env_resolution, env_resolution, GL_RG16F, GL_RG, GL_FLOAT },
                env_map_params
        );
        env_map_params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        gl::texture_cube_init(
                env_cube_map,
                { env_resolution, env_resolution, GL_RGB16F, GL_RGB, GL_FLOAT },
                env_map_params
        );
        env_map_params.generate_mipmap = true;
        int env_prefilter_width = 128;
        int env_prefilter_height = 128;
        gl::texture_cube_init(
                env_prefilter_map,
                { env_prefilter_width, env_prefilter_height, GL_RGB16F, GL_RGB, GL_FLOAT },
                env_map_params
        );
        // create unit cube for hdr conversions and env skybox
        gl::cube_default_init(env_cube, {});
        glm::mat4 cube_perspective = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 cube_views[] = {
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
        // create HDR env cube map
        gl::fbo_bind(hdr_env_fbo);
        glViewport(0, 0, env_resolution, env_resolution);
        gl::shader_use(hdr_to_cubemap_shader);
        gl::texture_update(hdr_to_cubemap_shader, env_hdr_texture);
        gl::shader_set_uniform4m(hdr_to_cubemap_shader, "perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            gl::shader_set_uniform4m(hdr_to_cubemap_shader, "view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_cube_map.id, 0);
            gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gl::draw(env_cube);
        }
        gl::texture_generate_mipmaps(env_cube_map);
        // create HDR env irradiance map
        gl::fbo_bind(hdr_env_fbo);
        glViewport(0, 0, 32, 32);
        hdr_env_rbo.width = 32;
        hdr_env_rbo.height = 32;
        gl::fbo_update(hdr_env_rbo);
        gl::shader_use(hdr_irradiance_shader);
        gl::texture_update(hdr_irradiance_shader, env_cube_map);
        gl::shader_set_uniform4m(hdr_irradiance_shader, "perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            gl::shader_set_uniform4m(hdr_irradiance_shader, "view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_irradiance_map.id, 0);
            gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gl::draw(env_cube);
        }
        // create HDR env prefilter map
        gl::fbo_bind(hdr_env_fbo);
        gl::shader_use(hdr_prefilter_convolution_shader);
        gl::texture_update(hdr_prefilter_convolution_shader, env_cube_map);
        gl::shader_set_uniform4m(hdr_prefilter_convolution_shader, "perspective", cube_perspective);
        int mip_w = env_prefilter_width * 2;
        int mip_h = env_prefilter_height * 2;
        for (int mip = 0 ; mip < env_prefilter_mip_levels ; mip++) {
            // resize framebuffer according to mip size.
            mip_w /= 2;
            mip_h /= 2;
            glViewport(0, 0, mip_w, mip_h);
            hdr_env_rbo.width = mip_w;
            hdr_env_rbo.height = mip_h;
            gl::fbo_update(hdr_env_rbo);
            // update roughness on each mip
            float roughness = (float) mip / (float) (env_prefilter_mip_levels - 1);
            gl::shader_set_uniformf(hdr_prefilter_convolution_shader, "roughness", roughness);
            float resolution = (float) env_resolution;
            gl::shader_set_uniformf(hdr_prefilter_convolution_shader, "resolution", resolution);
            // capture mip texture on each cube face
            for (int i = 0; i < 6; i++) {
                gl::shader_set_uniform4m(hdr_prefilter_convolution_shader, "view", cube_views[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env_prefilter_map.id, mip);
                gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                gl::draw(env_cube);
            }
        }
        // create env BRDF convolution map
        u32 brdf_conv_rect_vao = gl::vao_init();
        gl::fbo_bind(hdr_env_fbo);
        glViewport(0, 0, env_resolution, env_resolution);
        hdr_env_rbo.width = env_resolution;
        hdr_env_rbo.height = env_resolution;
        gl::fbo_update(hdr_env_rbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, env_brdf_convolution_map.id, 0);
        gl::shader_use(brdf_convolution_shader);
        gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl::draw_quad(brdf_conv_rect_vao);

        gl::fbo_unbind();

        // setup uniform buffers
        sunlight_ubo = gl::ubo_init(1, sizeof(sunlight));
        lights_ubo = gl::ubo_init(2, sizeof(point_lights));
        flashlight_ubo = gl::ubo_init(3, sizeof(flashlight));

        // setup sunlight
        sunlight.color = { 0, 0, 0, 1 };
        sunlight.direction = { 1, 1, 1, 0 };
        gl::ubo_update(sunlight_ubo, { 0, sizeof(sunlight), &sunlight });

        // setup lights
        point_lights[0].position = { -4, -1, 0, 1 };
        point_lights[0].color = { 1, 0, 0, 1 };
        point_lights[1].position = { 4, -1, 0, 1 };
        point_lights[1].color = { 0, 1, 0, 1 };
        point_lights[2].position = { -4, -1, 8, 1 };
        point_lights[2].color = { 0, 0, 1, 1 };
        point_lights[3].position = { 4, -1, 8, 1 };
        point_lights[3].color = { 1, 0, 1, 1 };
        gl::ubo_update(lights_ubo, { 0, sizeof(point_lights), point_lights.data() });

        // setup flashlight
        flashlight.position = { camera.position, 0 };
        flashlight.direction = { camera.front, 0 };
        flashlight.color = { 0, 0, 0, 1 };
        gl::ubo_update(flashlight_ubo, { 0, sizeof(flashlight), &flashlight });

        // setup light presentation
        point_light_present.color = { 0, 0, 1, 0.5 };
        gl::light_present_init(&point_light_present);

        // setup shadow
        gl::direct_shadow_init({ 1024, 1024 }, sunlight.direction);
        gl::point_shadow_init({ 1024, 1024 }, flashlight.position);

        // setup 3D model
        model = io::model_init("models/backpack/backpack.obj");
        model_shadow = gl::shadow_model_init(model);
        model_material = gl::material_init(
                true,
                "models/backpack/diffuse.jpg",
                null,
                null,
                null,
                null,
                "models/backpack/ao.jpg"
        );
        model_material.env_irradiance.id = env_irradiance_map.id;
        model_material.env_prefilter.id = env_prefilter_map.id;
        model_material.env_brdf_convolution.id = env_brdf_convolution_map.id;
        model_material.env_prefilter_mip_levels = env_prefilter_mip_levels;
        model_material.metallic_factor = 0;
        model_material.roughness_factor = 0;
        model_material.ao_factor = 1;

        // setup sphere
        gl::sphere_tbn_init(sphere);
        gl::sphere_default_init(sphere_shadow);
        sphere_materials.resize(sphere_transforms.size());
        {
            sphere_materials[0] = gl::material_init(
                    false,
                    "images/bumpy-rockface1-bl/albedo.png",
                    "images/bumpy-rockface1-bl/normal.png",
                    null,
                    "images/bumpy-rockface1-bl/metallic.png",
                    "images/bumpy-rockface1-bl/roughness.png",
                    "images/bumpy-rockface1-bl/ao.png"
            );
            sphere_materials[0].env_irradiance.id = env_irradiance_map.id;
            sphere_materials[0].env_prefilter.id = env_prefilter_map.id;
            sphere_materials[0].env_brdf_convolution.id = env_brdf_convolution_map.id;
            sphere_materials[0].env_prefilter_mip_levels = env_prefilter_mip_levels;
            sphere_materials[0].metallic_factor = 1;
            sphere_materials[0].roughness_factor = 1;
            sphere_materials[0].ao_factor = 1;

            sphere_materials[1] = gl::material_init(
                    false,
                    "images/rust_pbr/albedo.png",
                    "images/rust_pbr/normal.png",
                    null,
                    "images/rust_pbr/metallic.png",
                    "images/rust_pbr/roughness.png",
                    null
            );
            sphere_materials[1].env_irradiance.id = env_irradiance_map.id;
            sphere_materials[1].env_prefilter.id = env_prefilter_map.id;
            sphere_materials[1].env_brdf_convolution.id = env_brdf_convolution_map.id;
            sphere_materials[1].env_prefilter_mip_levels = env_prefilter_mip_levels;
            sphere_materials[1].metallic_factor = 1;
            sphere_materials[1].roughness_factor = 1;
            sphere_materials[1].ao_factor = 1;

            sphere_materials[2] = gl::material_init(
                    false,
                    "images/light-gold-bl/albedo.png",
                    null,
                    null,
                    null,
                    null,
                    null
            );
            sphere_materials[2].env_irradiance.id = env_irradiance_map.id;
            sphere_materials[2].env_prefilter.id = env_prefilter_map.id;
            sphere_materials[2].env_brdf_convolution.id = env_brdf_convolution_map.id;
            sphere_materials[2].env_prefilter_mip_levels = env_prefilter_mip_levels;
            sphere_materials[2].metallic_factor = 0;
            sphere_materials[2].roughness_factor = 0;
            sphere_materials[2].ao_factor = 1;
            sphere_materials[2].enable_normal = false;
        }

        // setup horizontal plane
        gl::cube_tbn_init(plane);
        plane_material = gl::material_init(
            false,
            null,
            null
        );
        plane_material.color = { 1, 1, 1, 1 };
        plane_material.env_irradiance.id = env_irradiance_map.id;
        plane_material.env_prefilter.id = env_prefilter_map.id;
        plane_material.env_brdf_convolution.id = env_brdf_convolution_map.id;
        plane_material.env_prefilter_mip_levels = env_prefilter_mip_levels;
        plane_material.metallic_factor = 0;
        plane_material.roughness_factor = 0.1;
        plane_material.ao_factor = 1;

        gl::shader_use(blur_shader);
        gl::shader_set_uniformf(blur_shader, "offset", blur_offset);

        gl::shader_use(0);
        gl::texture_unbind();
    }

    static void free() {
        gl::drawable_free(sphere);
        gl::drawable_free(sphere_shadow);
        gl::material_free(sphere_materials);

        gl::shader_free(hdr_to_cubemap_shader);
        gl::shader_free(hdr_irradiance_shader);
        gl::shader_free(hdr_prefilter_convolution_shader);
        gl::shader_free(brdf_convolution_shader);
        gl::shader_free(env_shader);
        gl::fbo_free(hdr_env_fbo);
        gl::fbo_free_attachment(hdr_env_rbo);
        gl::texture_free(env_hdr_texture.id);
        gl::texture_free(env_irradiance_map.id);
        gl::texture_free(env_cube_map.id);
        gl::texture_free(env_prefilter_map.id);
        gl::texture_free(env_brdf_convolution_map.id);
        gl::drawable_free(env_cube);

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

        gl::drawable_free(plane);
        gl::material_free(plane_material);

        gl::shader_free(material_shader);

        gl::ubo_free(sunlight_ubo);
        gl::ubo_free(lights_ubo);
        gl::ubo_free(flashlight_ubo);

        gl::outline_free();

        gl::light_present_free();

        io::model_free(model);
        io::shadow_model_free(model_shadow);

        gl::camera_free();

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

        gl::camera_view_update(camera);
        gl::camera_view_position_update(material_shader, camera.position);
    }

    static void simulate() {
        float t = begin_time;
        camera_control_update();
        // bind flashlight to camera
        flashlight.position = {camera.position, 0 };
        flashlight.direction = {camera.front, 0 };
        // rotate object each frame
        float f = 0.05f;
        sphere_transforms[0].rotation.y += f;
        sphere_transforms[1].rotation.y += f * 2;
        sphere_transforms[2].rotation.y += f * 4;
        // translate point lights up/down
        for (auto& point_light : point_lights) {
            point_light.position.y = 10 * sin(t/2) + 2;
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
        ui::checkbox("Irradiance Mapping", &enable_irradiance_mapping);
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
            gl::fbo_bind(msaa_fbo);
        } else {
            gl::fbo_bind(scene_fbo);
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
        {
            glDepthFunc(GL_LEQUAL);
            gl::shader_use(env_shader);
            gl::texture_update(env_shader, env_cube_map);
            gl::draw(env_cube);
            glDepthFunc(GL_LESS);
        }

        // upload shadow maps
        {
//            gl::shader_use(material_shader);
//            gl::direct_shadow_update(material_shader);
//            gl::point_shadow_update(material_shader);
        }

        // render and upload lights

        // sunlight
        {
            gl::ubo_update(sunlight_ubo, { 0, sizeof(sunlight), &sunlight });
        }
        // point lights
        {
            gl::ubo_update(lights_ubo, {0, sizeof(point_lights), point_lights.data() });
            // update light presentation
            for (auto& point_light : point_lights) {
                point_light_present.transform.translation = point_light.position;
                gl::light_present_update();
            }
        }
        // flashlight
        {
            gl::ubo_update(flashlight_ubo, {0, sizeof(flashlight), &flashlight });
        }

        // render material objects
        {
            gl::shader_use(material_shader);

            gl::material_update(material_shader, plane_material);
            gl::transform_update(material_shader, plane_transform);
            gl::draw(plane);

            for (int i = 0 ; i < sphere_transforms.size() ; i++) {
                gl::material_update(material_shader, sphere_materials[i]);
                gl::transform_update(material_shader, sphere_transforms[i]);
                gl::draw(sphere);
            }

            gl::material_update(material_shader, model_material);
            gl::transform_update(material_shader, model_transform);
            gl::draw(model.elements);
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
            gl::shader_use(material_shader);
            for (auto it = transparent_objects.rbegin() ; it != transparent_objects.rend() ; it++) {
                gl::transform_update(material_shader, it->second);
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
                gl::texture_update(blur_shader, final_render_target.view);
                gl::draw_quad(screen_vao);
                final_render_target = blur_colors[0];
            }
        }

        // render into screen
        {
            gl::fbo_unbind();
            gl::clear_display(1, 1, 1, 1, GL_COLOR_BUFFER_BIT);
            gl::shader_use(screen_shader);
            gl::texture_update(screen_shader, final_render_target.view);
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