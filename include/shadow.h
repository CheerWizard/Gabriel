#pragma once

#include <shader.h>
#include <buffers.h>
#include <transform.h>
#include <draw.h>
#include <model_loader.h>

namespace gl {

    struct shadow_props final {
        int width = 1024;
        int height = 1024;
    };

    struct shadow_vertex final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
    };

    struct shadow_mesh_vertices final {
        shadow_vertex* data;

        inline float* to_float() const { return (float*) &data->pos.x; }
    };

    struct shadow_mesh final {
        shadow_mesh_vertices vertices;
        u32 vertex_count;
        u32* indices;
        u32 index_count;
    };

    struct shadow_model final {
        std::vector<shadow_mesh> meshes;
    };

    struct shadow_drawable_model final {
        shadow_model model;
        gl::drawable_elements elements;
    };

    struct shadow_drawable_models final {
        shadow_model model;
        std::vector<drawable_elements> elements;
    };

    void shadow_end(u32 fbo, int viewport_width, int viewport_height);

    void direct_shadow_init(const shadow_props& props, const glm::vec3& light_dir);
    void direct_shadow_free();

    void direct_shadow_begin();

    void direct_shadow_update(const glm::vec3& light_dir);
    void direct_shadow_update(u32 shader);

    void direct_shadow_draw(transform& transform, const drawable_elements& drawable);

    void point_shadow_init(const shadow_props& props, const glm::vec3& light_pos);
    void point_shadow_free();

    void point_shadow_begin();

    void point_shadow_update(const glm::vec3& light_pos);
    void point_shadow_update(u32 shader);
    void point_shadow_draw(transform& transform, const drawable_elements& drawable);

    shadow_drawable_model shadow_model_init(const io::drawable_model& src);
    shadow_drawable_models shadow_model_init(const io::drawable_models& src);

    void shadow_model_free(shadow_drawable_model& drawable_model);
    void shadow_models_free(shadow_drawable_models& drawable_models);

}