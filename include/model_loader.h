#pragma once

#include <material_loader.h>
#include <vertex.h>
#include <draw.h>

namespace io {

    using namespace gl;

    struct VertexMesh final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
    };

    struct MeshVertices final {
        VertexMesh* data;

        inline float* to_float() const { return (float*) &data->pos.x; }
    };

    struct Mesh final {
        MeshVertices vertices;
        u32 vertex_count;
        u32* indices;
        u32 index_count;
        u32 material_index = 0;
    };

    struct Model final {
        std::vector<Mesh> meshes;
        std::unordered_map<u32, Material> materials;
    };

    struct DrawableModel final {
        io::Model model;
        DrawableElements elements;

        void init(
                const std::string& filepath,
                u32 flags = aiProcess_Triangulate
                        | aiProcess_FlipUVs
                        | aiProcess_CalcTangentSpace
                        | aiProcess_OptimizeMeshes
        );

        void free();
    };

    struct DrawableModels final {
        io::Model model;
        std::vector<DrawableElements> elements;

        void init(
                const std::string& filepath,
                u32 flags = aiProcess_Triangulate
                        | aiProcess_FlipUVs
                        | aiProcess_CalcTangentSpace
                        | aiProcess_OptimizeMeshes
        );

        void free();
    };

}