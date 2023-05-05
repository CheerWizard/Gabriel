#pragma once

#include <window.h>
#include <ui.h>
#include <debug_control.h>

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
#include <entity_control.h>
#include <terrain.h>

namespace gl {

    class App final {

    public:
        App();
        ~App();

        void run();

    private:
        void init_window();
        void init_scene();
        void init_pipeline();
        void init_camera();
        void init_light();
        void init_text();

        void free();

        void print_dt();

        void window_error(int error, const char* msg);
        void window_close();
        void window_resized(int w, int h);
        void window_positioned(int x, int y);

        void framebuffer_resized(int w, int h);

        void key_press(int key);
        void key_release(int key);

        void mouse_press(int button);
        void mouse_release(int button);
        void mouse_cursor(double x, double y);
        void mouse_scroll(double x, double y);

        static void entity_selected(ecs::Entity entity, double x, double y);
        static void entity_dragged(ecs::Entity entity, double x, double y);
        static void entity_hovered(ecs::Entity entity, double x, double y);

        void simulate();

        void render_postfx();

        void render_debug_screen();

        void render();

    private:
        bool running = true;
        float delta_time = 6;
        float begin_time = 0;

        bool enable_normal_mapping = true;
        bool enable_parallax_mapping = true;
        bool enable_geometry_debug = false;

        bool enable_hdr = true;
        bool enable_blur = false;
        bool enable_bloom = true;
        bool enable_ssao = true;

        ecs::Scene scene;

        Camera camera;

        LightPresent point_light_present;

        TerrainBuilder mTerrainBuilder;

        DisplacementImageMixer image_mixer;

        io::Model backpack_model;
        PBR_Entity backpack;

        ScreenRenderer* screen_renderer;
        PBR_Pipeline* pbr_pipeline;
        UI_Pipeline* ui_pipeline;
        DebugControlPipeline* debug_control_pipeline;
        ShadowPipeline* mShadowPipeline;

        HdrRenderer* hdr_renderer;

        BloomRenderer* bloom_renderer;

        BlurRenderer* blur_renderer;

        Animator human_animator;
        io::SkeletalModel human_model;
        PBR_Entity human;

        SphereTBN rock_sphere_geometry = { 128, 128 };
        PBR_Entity rock_sphere;

        PBR_Entity wood_sphere;

        PBR_Entity metal_sphere;

        int print_limiter = 100;

        EntityControl* entity_control;

        ecs::Entity text_label;
        Font* font_roboto_regular = null;

        DirectLight sunlight;
        std::array<PointLight, 4> pointLights;
        SpotLight flashlight;
    };

}