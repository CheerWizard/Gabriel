#include <skeletal_loader.h>
#include <assimp_core.h>

namespace io {

    VertexFormat SkeletalVertex::format = {
            { attr::pos, attr::uv, attr::normal, attr::tangent, attr::bone_id, attr::weight },
            sizeof(SkeletalVertex)
    };

    Bone::Bone(int id, const std::string& name, const aiNodeAnim *channel, const glm::mat4& offset)
    : id(id), name(name), offset(offset) {

        for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timestamp = channel->mPositionKeys[positionIndex].mTime;
            positions.emplace_back(AssimpCore::to_vec3(aiPosition), timestamp);
        }

        for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timestamp = channel->mRotationKeys[rotationIndex].mTime;
            rotations.emplace_back(AssimpCore::to_quat(aiOrientation), timestamp);
        }

        for (int keyIndex = 0; keyIndex < channel->mNumScalingKeys; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timestamp = channel->mScalingKeys[keyIndex].mTime;
            scales.emplace_back(AssimpCore::to_vec3(scale), timestamp);
        }

    }

    static SkeletalVertex parse_skeletal_vertex(aiMesh* mesh, u32 i) {
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

    static void parse_bone(
            SkeletalMesh& skeletal_mesh,
            aiMesh* mesh,
            int bone_index,
            std::unordered_map<std::string, Bone>& bones,
            aiNodeAnim* channel
    ) {
        std::string bone_name = mesh->mBones[bone_index]->mName.C_Str();
        int bone_id = bone_index;

        if (bones.find(bone_name) == bones.end()) {
            bones[bone_name] = {
                    bone_index,
                    bone_name,
                    channel,
                    AssimpCore::to_mat4(mesh->mBones[bone_index]->mOffsetMatrix)
            };
        } else {
            bone_id = bones[bone_name].id;
        }

        assert(bone_id != -1);
        auto weights = mesh->mBones[bone_index]->mWeights;
        int numWeights = mesh->mBones[bone_index]->mNumWeights;

        for (int weight_index = 0; weight_index < numWeights; ++weight_index)
        {
            int vertex_id = weights[weight_index].mVertexId;
            float weight = weights[weight_index].mWeight;
            assert(vertex_id <= skeletal_mesh.vertex_count);
            SkeletalVertex& vertex = skeletal_mesh.vertices.data[vertex_id];
            vertex.bone_id = { bone_id, bone_id, bone_id, bone_id };
            vertex.weight = { weight, weight, weight, weight };
        }
    }

    static SkeletalMesh parse_skeletal_mesh(aiMesh* mesh, std::unordered_map<std::string, Bone>& bones, aiAnimation* animation)
    {
        io::SkeletalMesh result;
        std::vector<u32> indices;

        result.vertex_count = mesh->mNumVertices;
        result.vertices.data = new SkeletalVertex[mesh->mNumVertices];

        for (u32 i = 0 ; i < mesh->mNumVertices ; i++) {
            result.vertices.data[i] = parse_skeletal_vertex(mesh, i);
        }

        for (int bone_index = 0; bone_index < mesh->mNumBones; ++bone_index)
        {
            parse_bone(result, mesh, bone_index, bones, animation->mChannels[bone_index]);
        }

        for (u32 i = 0 ; i < mesh->mNumFaces ; i++) {
            aiFace face = mesh->mFaces[i];
            for (u32 j = 0 ; j < face.mNumIndices ; j++)
                indices.push_back(face.mIndices[j]);
        }

        result.index_count = indices.size();
        result.indices = new u32[indices.size()];
        for (u32 i = 0 ; i < indices.size() ; i++) {
            result.indices[i] = indices[i];
        }

        return result;
    }

    static void parse_skeletal_meshes(
            aiNode* node, const aiScene* scene,
            SkeletalModel& model,
            const std::string& directory, u32 flags
    ) {
        auto& meshes = model.meshes;
        auto& materials = model.materials;

        for (u32 i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            io::SkeletalMesh result = parse_skeletal_mesh(mesh, model.bones, scene->mAnimations[0]);
            result.material_index = parse_material(materials, scene, mesh, directory, flags);
            meshes.push_back(result);
        }

        for (u32 i = 0 ; i < node->mNumChildren ; i++)
        {
            parse_skeletal_meshes(node->mChildren[i], scene, model, directory, flags);
        }
    }

    static void read_hierarchy_data(aiNode* src, AssimpNodeData& dest) {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpCore::to_mat4(src->mTransformation);

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            read_hierarchy_data(src->mChildren[i], newData);
            dest.children.push_back(newData);
        }
    }

    static SkeletalModel skeletal_model_read(
            const std::string& filepath,
            u32 flags
    ) {
        SkeletalModel model;

        AssimpCore::read_file([&filepath, &model, &flags](const aiScene* scene) {

            std::string directory = filepath.substr(0, filepath.find_last_of('/'));
            parse_skeletal_meshes(scene->mRootNode, scene, model, directory, flags);

            auto ai_animation = scene->mAnimations[0];
            model.animation.duration = ai_animation->mDuration;
            model.animation.ticks_per_second = ai_animation->mTicksPerSecond;
            read_hierarchy_data(scene->mRootNode, model.animation.root);

        }, filepath, flags);

        return model;
    }

    void DrawableSkeletalModel::init(const std::string &filepath, u32 flags) {
        model = skeletal_model_read(filepath, flags);
        model.animation.bones = &model.bones;
        // invalidate model vertices and indices to group together
        size_t mesh_count = model.meshes.size();
        u32 vertex_count = 0;
        u32 index_count = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = model.meshes[i];
            for (u32 j = 0 ; j < mesh.index_count ; j++) {
                mesh.indices[j] += vertex_count;
            }
            vertex_count += mesh.vertex_count;
            index_count += mesh.index_count;
        }

        drawable.vao.init();
        drawable.vao.bind();

        glGenBuffers(1, &drawable.vbo.id);
        glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(SkeletalVertex), null, GL_DYNAMIC_DRAW);
        drawable.vbo.set_format(SkeletalVertex::format);

        glGenBuffers(1, &drawable.ibo.id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertex_offset = 0;
        u32 index_offset = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = model.meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertex_count * sizeof(SkeletalVertex), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.index_count * sizeof(u32), mesh.indices);

            vertex_offset += mesh.vertex_count * sizeof(SkeletalVertex);
            index_offset += mesh.index_count * sizeof(u32);
        }

        drawable.index_count = index_count;
    }

    void DrawableSkeletalModels::init(const std::string &filepath, u32 flags) {
        model = skeletal_model_read(filepath, flags);
        model.animation.bones = &model.bones;

        size_t mesh_size = model.meshes.size();
        elements.resize(mesh_size);
        for (int i = 0 ; i < elements.size() ; i++) {
            auto& element = elements[i];
            auto& mesh = model.meshes[i];

            element.vao.init();
            element.vao.bind();

            glGenBuffers(1, &element.vbo.id);
            glBindBuffer(GL_ARRAY_BUFFER, element.vbo.id);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(SkeletalVertex), mesh.vertices.to_float(), GL_DYNAMIC_DRAW);
            element.vbo.set_format(SkeletalVertex::format);

            glGenBuffers(1, &element.ibo.id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element.ibo.id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(u32), mesh.indices, GL_DYNAMIC_DRAW);

            element.index_count = mesh.index_count;
        }
    }

    void DrawableSkeletalModel::free() {
        for (auto& mesh : model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }

        for (auto& material : model.materials) {
            material.second.free();
        }

        drawable.free();
    }

    void DrawableSkeletalModels::free() {
        for (auto& mesh : model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }

        for (auto& material : model.materials) {
            material.second.free();
        }

        for (auto& element : elements) {
            element.free();
        }
    }

}