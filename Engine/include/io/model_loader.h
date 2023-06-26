#pragma once

#include "material_loader.h"

#include "geometry/vertex.h"
#include "geometry/geometry.h"

namespace gl {

    struct GABRIEL_API VertexMesh final {
        VERTEX()
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
    };

    struct GABRIEL_API Mesh : Geometry<VertexMesh> {
        u32 materialIndex = 0;
    };

    struct GABRIEL_API Model final {
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