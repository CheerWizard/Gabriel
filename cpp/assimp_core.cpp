#include <assimp_core.h>

namespace gl {

    glm::vec3 AssimpCore::to_vec3(const aiVector3D& vec3) {
        return { vec3.x, vec3.y, vec3.z };
    }

    glm::quat AssimpCore::to_quat(const aiQuaternion& quat) {
        return { quat.w, quat.x, quat.y, quat.z };
    }

    glm::mat4 AssimpCore::to_mat4(const aiMatrix4x4& mat) {
        return {
                mat.a1, mat.a2, mat.a3, mat.a4,
                mat.b1, mat.b2, mat.b3, mat.b4,
                mat.c1, mat.c2, mat.c3, mat.c4,
                mat.d1, mat.d2, mat.d3, mat.d4,
        };
    }

    void AssimpCore::read_file(
            const std::function<void(const aiScene*)>& handler,
            const std::string &filepath, u32 flags
    ) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            print_err("AssimpCore: failed to read file " << filepath);
            return;
        }

        handler(scene);
    }
}