#pragma once

#include <entity.h>
#include <transform.h>
#include <material.h>
#include <buffers.h>
#include <light.h>
#include <shadow.h>
#include <outline.h>
#include <ssao.h>
#include <skeletal_renderer.h>
#include <environment.h>
#include <transparency.h>

namespace gl {

    struct PBR_Pixel final {
        ecs::EntityID entity_id = ecs::InvalidEntity;
    };

    component(PBR_Component_Forward) {};
    component(PBR_Component_Deferred) {};
    component(PBR_Component_ForwardCull) {};
    component(PBR_Component_DeferredCull) {};

    component(PBR_SkeletalComponent_Forward) {};
    component(PBR_SkeletalComponent_Deferred) {};
    component(PBR_SkeletalComponent_ForwardCull) {};
    component(PBR_SkeletalComponent_DeferredCull) {};

    component(PBR_Component_Transparent) {};

    struct PBR_Entity : ecs::Entity {

        PBR_Entity() : ecs::Entity() {}

        PBR_Entity(ecs::Scene* scene, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
        : ecs::Entity(scene) {
            add_component<Transform>(translation, rotation, scale);
            add_component<DrawableElements>();
            add_component<Material>();
        }

        Transform* transform() {
            return scene->get_component<Transform>(id);
        }

        DrawableElements* drawable() {
            return scene->get_component<DrawableElements>(id);
        }

        Material* material() {
            return scene->get_component<Material>(id);
        }

    };

    struct PBR_ForwardRenderer final {
        ImageBuffer render_target;
        int samples = 1;
        ImageBuffer direct_shadow_map;
        ImageBuffer point_shadow_map;
        float far_plane = 25;

        inline Shader& get_shader();
        inline FrameBuffer& get_current_frame();

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void set_camera_pos(glm::vec3& camera_pos);
        void set_samples(int samples);

        void read_pixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material);
        void render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& light_space);

        void update(Environment* env);

        void blit_color_depth(int w, int h, u32 src_color_fbo, u32 src_depth_fbo);
        void blit_entity_id(int w, int h, u32 src_fbo, int src_entity_id);

    private:
        VertexArray vao;
        Shader shader;
        FrameBuffer fbo;
        FrameBuffer msaa_fbo;
        FrameBuffer current_fbo;
    };

    struct PBR_GBuffer final {
        ImageBuffer position;
        ImageBuffer normal;
        ImageBuffer albedo;
        ImageBuffer pbr_params;
        ImageBuffer shadow_proj_coords;
        ImageBuffer object_id;
        ImageBuffer view_position;
        ImageBuffer view_normal;
    };

    struct PBR_DeferredRenderer final {
        ImageBuffer render_target;
        PBR_GBuffer gbuffer;
        int samples = 1;
        bool enable_ssao = true;
        ImageBuffer direct_shadow_map;
        ImageBuffer point_shadow_map;
        float far_plane = 25;
        SsaoRenderer* ssao_renderer;

        inline FrameBuffer& get_geometry_fbo() { return geometry_fbo; }
        inline FrameBuffer& get_light_fbo() { return light_fbo; }
        inline const ImageBuffer& get_ssao_buffer() const { return ssao_renderer->get_render_target(); }

        PBR_DeferredRenderer(int w, int h);
        ~PBR_DeferredRenderer();

        void resize(int w, int h);

        void set_camera_pos(glm::vec3& camera_pos);
        void set_samples(int samples);

        void read_pixel(PBR_Pixel& pixel, int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material);
        void render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& light_space);

        void update(Environment* env);

    private:
        DrawableQuad drawable;
        Shader geometry_shader;
        FrameBuffer geometry_fbo;
        FrameBuffer geometry_msaa_fbo;
        FrameBuffer current_geometry_fbo;
        Shader light_shader;
        FrameBuffer light_fbo;
    };

    struct PBR_Skeletal_ForwardRenderer : SkeletalRenderer {
        void init();
        void update(Environment* env);
    };

    struct PBR_Skeletal_DeferredRenderer : SkeletalRenderer {
        void init();
        void update(Environment* env);
    };

    struct PBR_Pipeline final {
        ecs::Scene* scene;
        Environment env;
        DirectLight sunlight;
        std::array<PointLight, 4> point_lights;
        SpotLight flashlight;

        PBR_Pipeline(ecs::Scene* scene, int w, int h);
        ~PBR_Pipeline();

        inline const ImageBuffer& get_render_target() const {
            return pbr_forward_renderer.render_target;
        }

        inline const PBR_GBuffer& get_gbuffer() const {
            return pbr_deferred_renderer->gbuffer;
        }

        inline const TransparentBuffer& get_transparent_buffer() const {
            return transparent_renderer.transparent_buffer;
        }

        inline const ImageBuffer& get_ssao_buffer() const {
            return pbr_deferred_renderer->get_ssao_buffer();
        }

        void set_samples(int samples);

        void init_hdr_env(const char* filepath, bool flip_uv);
        void generate_env();

        void resize(int w, int h);

        void set_camera_pos(glm::vec3& camera_pos);

        void read_pixel(PBR_Pixel& pixel, int x, int y);

        void render();

        void update_flashlight();
        void update_sunlight();
        void update_pointlights();

    private:
        void forward_rendering();
        void forward_default_render();
        void forward_face_cull_render();
        void deferred_rendering();

    private:
        glm::ivec2 resolution;

        PBR_ForwardRenderer pbr_forward_renderer;
        PBR_DeferredRenderer* pbr_deferred_renderer;

        PBR_Skeletal_ForwardRenderer skeletal_forward_renderer;
        PBR_Skeletal_DeferredRenderer skeletal_deferred_renderer;

        TransparentRenderer transparent_renderer;

        DirectShadowRenderer direct_shadow_renderer;
        PointShadowRenderer point_shadow_renderer;

        OutlineRenderer outline_renderer;

        EnvRenderer env_renderer;

        UniformBuffer sunlight_ubo;
        UniformBuffer lights_ubo;
        UniformBuffer flashlight_ubo;
    };

}