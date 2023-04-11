#pragma once

#include <vertex.h>
#include <draw.h>
#include <material_loader.h>
#include <geometry.h>

namespace io {

    using namespace gl;

    struct SkeletalVertex final {
        decl_vertex
        glm::fvec3 pos = { 0, 0, 0 };
        glm::fvec2 uv = { 0, 0 };
        glm::fvec3 normal = { 0, 0, 0 };
        glm::fvec3 tangent = { 0, 0, 0 };
        glm::ivec4 bone_id = { -1, -1, -1, -1 };
        glm::fvec4 weight = {0, 0, 0, 0 };
    };

    struct SkeletalMesh : Geometry<SkeletalVertex> {
        u32 material_index = 0;
    };

    struct KeyPosition final {
        glm::vec3 position;
        float timestamp;

        KeyPosition() = default;
        KeyPosition(const glm::vec3& position, float timestamp) : position(position), timestamp(timestamp) {}
    };

    struct KeyRotation final {
        glm::quat orientation;
        float timestamp;

        KeyRotation() = default;
        KeyRotation(const glm::quat& orientation, float timestamp) : orientation(orientation), timestamp(timestamp) {}
    };

    struct KeyScale final {
        glm::vec3 scale;
        float timestamp;

        KeyScale() = default;
        KeyScale(const glm::vec3& scale, float timestamp) : scale(scale), timestamp(timestamp) {}
    };

    struct Bone final {
        int id;
        std::string name;
        glm::mat4 offset;
        glm::mat4 transform = glm::mat4(1.0);
        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale> scales;

        Bone() = default;
        Bone(int id, const std::string& name, const aiNodeAnim* channel, const glm::mat4& offset);
    };

    struct AssimpNodeData final {
        std::string name;
        glm::mat4 transformation;
        std::vector<AssimpNodeData> children;
    };

    struct Animation final {
        float duration;
        int ticks_per_second;
        AssimpNodeData root;
        std::unordered_map<std::string, io::Bone>* bones = null;
    };

    struct SkeletalModel final {
        std::vector<SkeletalMesh> meshes;
        std::unordered_map<u32, Material> materials;
        std::unordered_map<std::string, Bone> bones;
        Animation animation;

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