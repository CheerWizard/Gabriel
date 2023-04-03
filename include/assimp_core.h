#pragma once

#include <primitives.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <unordered_map>
#include <functional>

namespace gl {

    struct AssimpCore final {
        static glm::mat4 to_mat4(const aiMatrix4x4& mat);
        static glm::vec3 to_vec3(const aiVector3D& vec3);
        static glm::quat to_quat(const aiQuaternion& quat);
        static void read_file(const std::function<void(const aiScene*)>& handler, const std::string& filepath, u32 flags);
    };

}