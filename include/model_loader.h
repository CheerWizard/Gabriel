#pragma once

#include <primitives.h>
#include <vertex.h>
#include <material.h>
#include <draw.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

namespace io {

    using namespace gl;

    struct vertex_mesh final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
        glm::fvec3 bitangent = { 0, 0, 0 };
    };

    struct mesh_vertices final {
        vertex_mesh* data;

        inline float* to_float() const { return (float*) &data->pos.x; }
    };

    struct mesh final {
        mesh_vertices vertices;
        u32 vertex_count;
        u32* indices;
        u32 index_count;
        u32 material_index = 0;
    };

    struct model final {
        std::vector<mesh> meshes;
        std::unordered_map<u32, material> materials;
    };

    struct drawable_model final {
        io::model model;
        gl::drawable_elements elements;
    };

    struct drawable_models final {
        io::model model;
        std::vector<gl::drawable_elements> elements;
    };

    drawable_model model_init(const std::string& filepath, u32 flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    drawable_models models_init(const std::string& filepath, u32 flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    void model_free(drawable_model& drawable_model);
    void models_free(drawable_models& drawable_models);

}