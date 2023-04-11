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
#include <polygon_visual.h>
#include <normal_visual.h>

namespace gl {

    struct PBR_Pixel final {
        ecs::EntityID entity_id = ecs::InvalidEntity;
    };

    struct PBR_Component_Forward : ecs::Component {};
    struct PBR_Component_Deferred : ecs::Component {};
    struct PBR_Component_ForwardCull : ecs::Component {};
    struct PBR_Component_DeferredCull : ecs::Component {};

    struct PBR_SkeletalComponent_Forward : ecs::Component {};
    struct PBR_SkeletalComponent_Deferred : ecs::Component {};
    struct PBR_SkeletalComponent_ForwardCull : ecs::Component {};
    struct PBR_SkeletalComponent_DeferredCull : ecs::Component {};

    struct PBR_Component_Transparent : ecs::Component {};

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
        SSAO_Pass* ssao_pass;
        ImageBuffer direct_shadow_map;
        ImageBuffer point_shadow_map;
        float far_plane = 25;

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

        void blit(int w, int h, u32 dest_fbo, int dest_entity_id);

    private:
        VertexArray vao;
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
        ecs::Scene* scene = null;
        Environment env;
        DirectLight sunlight;
        std::array<PointLight, 4> point_lights;
        SpotLight flashlight;

        inline ImageBuffer& render_target() {
            return pbr_forward_renderer.render_target;
        }

        inline PBR_GBuffer& gbuffer() {
            return pbr_deferred_renderer.gbuffer;
        }

        inline TransparentBuffer& transparent_buffer() {
            return transparent_renderer.transparent_buffer;
        }

        void init(int w, int h);
        void free();

        void set_samples(int samples);
        void set_ssao_pass(SSAO_Pass* ssao_pass);

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
        void deferred_rendering();

    private:
        glm::ivec2 resolution;

        PBR_ForwardRenderer pbr_forward_renderer;
        PBR_DeferredRenderer pbr_deferred_renderer;

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

        PolygonVisualRenderer polygon_visual_renderer;
        NormalVisualRenderer normal_visual_renderer;
    };

}