#pragma once

#include <transform.h>
#include <material.h>
#include <buffers.h>
#include <frame.h>
#include <draw.h>
#include <light.h>
#include <shadow.h>
#include <outline.h>
#include <ssao.h>

namespace gl {

    struct Entity {
        u32 id;
        Transform transform;

        Entity() = default;

        Entity(u32 id, const Transform& transform) : id(id), transform(transform) {}
    };

    struct Material_Entity : Entity {
        Material material;

        Material_Entity() = default;

        Material_Entity(u32 id, const Transform& transform)
        : Entity(id, transform) {}

        Material_Entity(u32 id, const Transform& transform, const Material& material)
        : Entity(id, transform), material(material) {}

        void update(Shader& shader);
    };

    struct PBR_Entity : Material_Entity {
        DrawableElements drawable;

        PBR_Entity() = default;

        PBR_Entity(u32 id, const Transform& transform)
        : Material_Entity(id, transform) {}

        PBR_Entity(u32 id, const Transform& transform, const Material& material)
        : Material_Entity(id, transform, material) {}

        void free();
    };

    struct PBR_EntityGroup final {
        std::vector<Material_Entity> entities;
        DrawableElements drawable;

        PBR_EntityGroup() = default;

        PBR_EntityGroup(const std::initializer_list<Material_Entity>& entities) : entities(entities) {}

        void free();
    };

    struct PBR_Pixel final {
        u32 object_id = 0;
    };

    struct TransparentBuffer final {
        Texture accumulation;
        Texture revealage;
    };

    struct TransparentRenderer final {
        Texture render_target;
        TransparentBuffer transparent_buffer;

        void init(Shader& shader, int w, int h);
        void free();

        void begin();
        void end();

        void resize(int w, int h);

        void render(PBR_Entity& entity);
        void render(PBR_EntityGroup& group);

    private:
        VertexArray vao;
        Shader shader;
        FrameBuffer fbo;
        Shader composite_shader;
        FrameBuffer composite_fbo;
    };

    struct SkeletalRenderer {

        void free();

        void begin();

        void set_camera_pos(glm::vec3& camera_pos);
        void update_bones(std::vector<glm::mat4>& bones);

        void render(PBR_Entity& entity);
        void render(PBR_EntityGroup& group);

    protected:
        Shader shader;
    };

    struct PBR_Skeletal_ForwardRenderer : SkeletalRenderer {
        void init();
        void update(Environment* env);
    };

    struct PBR_Skeletal_DeferredRenderer : SkeletalRenderer {
        void init();
        void update(Environment* env);
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

        void render(PBR_Entity& entity);
        void render(PBR_EntityGroup& group);

        void render(PBR_Entity& entity, glm::mat4& light_space);
        void render(PBR_EntityGroup& group, glm::mat4& light_space);

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

        void render(PBR_Entity& entity);
        void render(PBR_EntityGroup& group);

        void render(PBR_Entity& entity, glm::mat4& light_space);
        void render(PBR_EntityGroup& group, glm::mat4& light_space);

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

    struct Scene final {
        Environment env;

        std::vector<PBR_Entity*> forward_entities;
        std::vector<PBR_EntityGroup*> forward_groups;

        std::vector<PBR_Entity*> culled_forward_entities;
        std::vector<PBR_EntityGroup*> culled_forward_groups;

        std::vector<PBR_Entity*> deferred_entities;
        std::vector<PBR_EntityGroup*> deferred_groups;

        std::vector<PBR_Entity*> culled_deferred_entities;
        std::vector<PBR_EntityGroup*> culled_deferred_groups;

        std::vector<PBR_Entity*> skeletal_forward_entities;
        std::vector<PBR_EntityGroup*> skeletal_forward_groups;

        std::vector<PBR_Entity*> skeletal_deferred_entities;
        std::vector<PBR_EntityGroup*> skeletal_deferred_groups;

        std::vector<PBR_Entity*> culled_skeletal_forward_entities;
        std::vector<PBR_EntityGroup*> culled_skeletal_forward_groups;

        std::vector<PBR_Entity*> culled_skeletal_deferred_entities;
        std::vector<PBR_EntityGroup*> culled_skeletal_deferred_groups;

        std::vector<PBR_Entity*> transparent_entities;
        std::vector<PBR_EntityGroup*> transparent_groups;

        std::vector<DirectShadow*> direct_shadows;
        std::vector<PointShadow*> point_shadows;

        std::vector<Outline> outlines;
        std::vector<PBR_Entity*> outline_entities;
        std::vector<PBR_EntityGroup*> outline_groups;
    };

    struct PBR_Pipeline final {
        Scene* scene;

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
    };

}