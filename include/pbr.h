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
        u32 object_id = 0;
    };

    using namespace ecs;

    struct PBR_Component : Component {
        u32 object_id;
        Transform transform;
        Material* material;
        DrawableElements* drawable;

        PBR_Component() = default;

        explicit PBR_Component(u32 object_id) : object_id(object_id) {}

        PBR_Component(u32 object_id, const Transform &transform)
        : object_id(object_id), transform(transform) {}

        PBR_Component(u32 object_id, const Transform &transform, Material *material)
        : object_id(object_id), transform(transform), material(material) {}

        PBR_Component(u32 object_id, const Transform &transform, Material *material, DrawableElements *drawable)
        : object_id(object_id), transform(transform), material(material), drawable(drawable) {}
    };

#define decl_pbr_component(type) \
struct type : PBR_Component { \
    explicit type(u32 object_id) : PBR_Component(object_id) {} \
    type(u32 object_id, const Transform& transform) : PBR_Component(object_id, transform) {} \
    type(u32 object_id, const Transform& transform, Material* material) : PBR_Component(object_id, transform, material) {} \
    type(u32 object_id, const Transform& transform, Material* material, DrawableElements* drawable) : PBR_Component(object_id, transform, material, drawable) {} \
};

    decl_pbr_component(PBR_Component_Forward)
    decl_pbr_component(PBR_Component_Deferred)
    decl_pbr_component(PBR_Component_ForwardCull)
    decl_pbr_component(PBR_Component_DeferredCull)

    decl_pbr_component(PBR_SkeletalComponent_Forward)
    decl_pbr_component(PBR_SkeletalComponent_Deferred)
    decl_pbr_component(PBR_SkeletalComponent_ForwardCull)
    decl_pbr_component(PBR_SkeletalComponent_DeferredCull)

    decl_pbr_component(PBR_Component_Transparent)

    template<typename T>
    struct PBR_Entity : Entity {

        PBR_Entity() : Entity() {}

        PBR_Entity(Scene* scene, const Transform& transform) : Entity(scene) {
            add_component<T>(id, transform);
        }

        PBR_Entity(Scene* scene, const Transform& transform, Material* material) : Entity(scene) {
            add_component<T>(id, transform, material);
        }

        PBR_Entity(Scene* scene, const Transform& transform, Material* material, DrawableElements* drawable) : Entity(scene) {
            add_component<T>(id, transform, material, drawable);
        }

        T* component() {
            return scene->get_component<T>(id);
        }

    };

    struct PBR_ForwardRenderer final {
        Texture render_target;
        int samples = 1;
        Texture direct_shadow_map;
        Texture point_shadow_map;
        float far_plane = 25;

        inline Shader& get_shader();
        inline FrameBuffer& get_current_frame();

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void set_camera_pos(glm::vec3& camera_pos);

        void set_samples(int samples);

        PBR_Pixel read_pixel(int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(PBR_Component* pbr_component);
        void render(PBR_Component* pbr_component, glm::mat4& light_space);

        void update(Environment* env);

    private:
        VertexArray vao;
        Shader shader;
        FrameBuffer fbo;
        FrameBuffer msaa_fbo;
        FrameBuffer current_fbo;
    };

    struct PBR_GBuffer final {
        Texture position;
        Texture normal;
        Texture albedo;
        Texture pbr_params;
        Texture shadow_proj_coords;
        Texture object_id;
        Texture view_position;
        Texture view_normal;
    };

    struct PBR_DeferredRenderer final {
        Texture render_target;
        PBR_GBuffer gbuffer;
        int samples = 1;
        bool enable_ssao = true;
        SSAO_Pass* ssao_pass;
        Texture direct_shadow_map;
        Texture point_shadow_map;
        float far_plane = 25;

        void init(int w, int h);

        void free();

        void resize(int w, int h);

        void set_camera_pos(glm::vec3& camera_pos);

        void set_samples(int samples);

        PBR_Pixel read_pixel(int x, int y);

        void bind();
        void unbind();

        void begin();

        void render(PBR_Component* pbr_component);
        void render(PBR_Component* pbr_component, glm::mat4& light_space);

        void update(Environment* env);

        void blit(int w, int h, u32 dest_fbo, int dest_object_id);

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
        Scene* scene;
        Environment env;
        DirectLight sunlight;
        std::array<PointLight, 4> point_lights;
        SpotLight flashlight;

        inline Texture& render_target() {
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

        PBR_Pixel read_pixel(int x, int y);

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
    };

}