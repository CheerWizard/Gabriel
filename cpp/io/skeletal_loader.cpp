#include <io/skeletal_loader.h>

namespace gl {

    VertexFormat SkeletalVertex::format = {
            {
                Attributes::POS3,
                Attributes::UV,
                Attributes::NORMAL,
                Attributes::TANGENT,
                Attributes::BONE_ID,
                Attributes::WEIGHT
            },
            sizeof(SkeletalVertex)
    };

    Bone::Bone(int id, const std::string& name, const aiNodeAnim* channel, const glm::mat4& offset)
    : id(id), name(name), offset(offset) {

        for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timestamp = channel->mPositionKeys[positionIndex].mTime;
            positions.emplace_back(AssimpCore::toVec3(aiPosition), timestamp);
        }

        for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timestamp = channel->mRotationKeys[rotationIndex].mTime;
            rotations.emplace_back(AssimpCore::toQuat(aiOrientation), timestamp);
        }

        for (int keyIndex = 0; keyIndex < channel->mNumScalingKeys; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timestamp = channel->mScalingKeys[keyIndex].mTime;
            scales.emplace_back(AssimpCore::toVec3(scale), timestamp);
        }

    }

    static SkeletalVertex parseSkeletalVertex(aiMesh* mesh, u32 i) {
        SkeletalVertex vertex;

        vertex.pos = {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
        };

        if (mesh->mTextureCoords[0]) {
            vertex.uv = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
            };
        }

        vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
        };

        vertex.tangent = {
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z,
        };

        return vertex;
    }

    static void parseBone(
            SkeletalMesh& skeletalMesh,
            aiMesh* mesh,
            int boneIndex,
            std::unordered_map<std::string, Bone>& bones,
            aiNodeAnim* channel
    ) {
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        int boneId = boneIndex;

        if (bones.find(boneName) == bones.end()) {
            bones[boneName] = {
                    boneIndex,
                    boneName,
                    channel,
                    AssimpCore::toMat4(mesh->mBones[boneIndex]->mOffsetMatrix)
            };
        } else {
            boneId = bones[boneName].id;
        }

        assert(boneId != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= skeletalMesh.vertices.count);
            SkeletalVertex& vertex = skeletalMesh.vertices[vertexId];
            vertex.bone_id = { boneId, boneId, boneId, boneId };
            vertex.weight = { weight, weight, weight, weight };
        }
    }

    static SkeletalMesh parseSkeletalMesh(aiMesh* mesh, std::unordered_map<std::string, Bone>& bones, aiAnimation* animation)
    {
        SkeletalMesh result;
        std::vector<u32> indices;

        result.vertices.init((int) mesh->mNumVertices);

        for (int i = 0 ; i < mesh->mNumVertices ; i++) {
            result.vertices[i] = parseSkeletalVertex(mesh, i);
        }

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            parseBone(result, mesh, boneIndex, bones, animation->mChannels[boneIndex]);
        }

        for (u32 i = 0 ; i < mesh->mNumFaces ; i++) {
            aiFace face = mesh->mFaces[i];
            for (u32 j = 0 ; j < face.mNumIndices ; j++)
                indices.push_back(face.mIndices[j]);
        }

        result.indices.init((int) indices.size());
        for (int i = 0 ; i < indices.size() ; i++) {
            result.indices[i] = indices[i];
        }

        return result;
    }

    static void parseSkeletalMeshes(
            aiNode* node, const aiScene* scene,
            SkeletalModel& model,
            const std::string& directory, u32 flags
    ) {
        auto& meshes = model.meshes;
        auto& materials = model.materials;

        for (u32 i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            SkeletalMesh result = parseSkeletalMesh(mesh, model.bones, scene->mAnimations[0]);
            result.materialIndex = MaterialLoader::parseMaterial(materials, scene, mesh, directory, flags);
            meshes.push_back(result);
        }

        for (u32 i = 0 ; i < node->mNumChildren ; i++)
        {
            parseSkeletalMeshes(node->mChildren[i], scene, model, directory, flags);
        }
    }

    static void readHierarchyData(aiNode* src, AssimpNodeData& dest) {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpCore::toMat4(src->mTransformation);

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            readHierarchyData(src->mChildren[i], newData);
            dest.children.push_back(newData);
        }
    }

    void SkeletalModel::init(DrawableElements& drawable) {
        // invalidate model vertices and indices to group together
        size_t meshCount = meshes.size();
        u32 vertexCount = 0;
        u32 indexCount = 0;
        for (u32 i = 0 ; i < meshCount ; i++) {
            auto& mesh = meshes[i];
            for (int j = 0 ; j < mesh.indices.count ; j++) {
                mesh.indices[j] += vertexCount;
            }
            vertexCount += mesh.vertices.count;
            indexCount += mesh.indices.count;
        }

        drawable.vao.init();
        drawable.vao.bind();

        glGenBuffers(1, &drawable.vbo.id);
        glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(SkeletalVertex), null, GL_DYNAMIC_DRAW);
        drawable.vbo.setFormat(SkeletalVertex::format);

        glGenBuffers(1, &drawable.ibo.id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertexOffset = 0;
        u32 indexOffset = 0;
        for (u32 i = 0 ; i < meshCount ; i++) {
            auto& mesh = meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
            glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, mesh.vertices.size(), mesh.vertices.toFloat());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexOffset, mesh.indices.size(), mesh.indices.indices);

            vertexOffset += mesh.vertices.size();
            indexOffset += mesh.indices.size();
        }

        drawable.type = GL_TRIANGLES;
        drawable.strips = 1;
        drawable.verticesPerStrip = indexCount;
    }

    void SkeletalModel::free() {
        for (auto& mesh : meshes) {
            mesh.free();
        }
        for (auto& material : materials) {
            material.second.free();
        }
    }

    void SkeletalModel::generate(const std::string &filepath, u32 flags) {
        AssimpCore::readFile([&filepath, this, &flags](const aiScene* scene) {

            std::string directory = filepath.substr(0, filepath.find_last_of('/'));
            parseSkeletalMeshes(scene->mRootNode, scene, *this, directory, flags);

            auto aiAnimation = scene->mAnimations[0];
            animation.duration = aiAnimation->mDuration;
            animation.ticksPerSecond = aiAnimation->mTicksPerSecond;
            readHierarchyData(scene->mRootNode, animation.root);
            animation.bones = &bones;

        }, filepath, flags);
    }

}