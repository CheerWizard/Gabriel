#pragma once

#include <primitives.h>
#include <material.h>
#include <assimp_core.h>

#include <sstream>

namespace io {

    using namespace gl;

    u32 parse_material(
            std::unordered_map<u32, Material>& materials,
            const aiScene* scene, aiMesh* mesh,
            const std::string& directory, u32 flags
    );

}