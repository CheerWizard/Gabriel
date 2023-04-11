#pragma once

#include <material_loader.h>
#include <vertex.h>
#include <draw.h>
#include <geometry.h>

namespace io {

    using namespace gl;

    struct VertexMesh final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
    };

    struct Mesh : Geometry<VertexMesh> {
        u32 material_index = 0;
    };

    struct Model final {
        std::vector<Mesh> meshes;
        std::unordered_map<u32, Material> materials;

        void init(DrawableElements& drawable);
        void free();

        void generate(
                const std::string& filepath,
                u32 flags = aiProcess_Triangulate
                            | aiProcess_FlipUVs
                            | aiProcess_CalcTangentSpace
                            | aiProcess_OptimizeMeshes
        );
    };

}