#pragma once

#include <core/common.h>
#include <core/assimp_core.h>

#include <features/material.h>

#include <sstream>

namespace gl {

    struct MaterialLoader final {
        static u32 parseMaterial(
                std::unordered_map<u32, Material>& materials,
                const aiScene* scene, aiMesh* mesh,
                const std::string& directory, u32 flags
        );
    };

}