#include <model_loader.h>

#include <geometry.h>

#include <sstream>
#include <unordered_map>

namespace io {

    using namespace gl;

    VertexFormat VertexMesh::format = {
            { attr::pos, attr::uv, attr::normal, attr::tangent },
            sizeof(VertexMesh)
    };

    static VertexMesh parse_vertex(aiMesh* mesh, u32 i) {
        VertexMesh vertex;

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

    static Mesh parse_mesh(aiMesh *mesh)
    {
        io::Mesh result;
        std::vector<u32> indices;

        result.vertex_count = mesh->mNumVertices;
        result.vertices.data = new VertexMesh[mesh->mNumVertices];

        for (u32 i = 0 ; i < mesh->mNumVertices ; i++) {
            result.vertices.data[i] = parse_vertex(mesh, i);
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

    static void parse_meshes(
            aiNode* node, const aiScene* scene,
            Model& model,
            const std::string& directory, u32 flags
    ) {
        auto& meshes = model.meshes;
        auto& materials = model.materials;

        for (u32 i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            io::Mesh result = parse_mesh(mesh);
            result.material_index = parse_material(materials, scene, mesh, directory, flags);
            meshes.push_back(result);
        }

        for (u32 i = 0 ; i < node->mNumChildren ; i++)
        {
            parse_meshes(node->mChildren[i], scene, model, directory, flags);
        }
    }

    static Model model_read(
            const std::string& filepath,
            u32 flags
    ) {
        Model model;

        AssimpCore::read_file([&model, &filepath, &flags](const aiScene* scene) {
            std::string directory = filepath.substr(0, filepath.find_last_of('/'));
            parse_meshes(scene->mRootNode, scene, model, directory, flags);
        }, filepath, flags);

        return model;
    }

    void DrawableModel::init(const std::string &filepath, u32 flags) {
        model = model_read(filepath, flags);
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
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(VertexMesh), null, GL_DYNAMIC_DRAW);
        drawable.vbo.set_format(VertexMesh::format);

        glGenBuffers(1, &drawable.ibo.id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertex_offset = 0;
        u32 index_offset = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = model.meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertex_count * sizeof(VertexMesh), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.index_count * sizeof(u32), mesh.indices);

            vertex_offset += mesh.vertex_count * sizeof(VertexMesh);
            index_offset += mesh.index_count * sizeof(u32);
        }

        drawable.index_count = index_count;
    }

    void DrawableModels::init(const std::string &filepath, u32 flags) {
        model = model_read(filepath, flags);

        size_t mesh_size = model.meshes.size();
        elements.resize(mesh_size);
        for (int i = 0 ; i < elements.size() ; i++) {
            auto& element = elements[i];
            auto& mesh = model.meshes[i];

            element.vao.init();
            element.vao.bind();

            glGenBuffers(1, &element.vbo.id);
            glBindBuffer(GL_ARRAY_BUFFER, element.vbo.id);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(VertexMesh), mesh.vertices.to_float(), GL_DYNAMIC_DRAW);
            element.vbo.set_format(VertexMesh::format);

            glGenBuffers(1, &element.ibo.id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element.ibo.id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(u32), mesh.indices, GL_DYNAMIC_DRAW);

            element.index_count = mesh.index_count;
        }
    }

    void DrawableModel::free() {
        for (auto& mesh : model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }

        for (auto& material : model.materials) {
            material.second.free();
        }

        drawable.free();
    }

    void DrawableModels::free() {
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