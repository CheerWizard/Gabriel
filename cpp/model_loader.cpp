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

        result.vertices.init(mesh->mNumVertices);

        for (int i = 0 ; i < mesh->mNumVertices ; i++) {
            result.vertices[i] = parse_vertex(mesh, i);
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

    void Model::init(DrawableElements& drawable) {
        // invalidate model vertices and indices to group together
        size_t mesh_count = meshes.size();
        u32 vertex_count = 0;
        u32 index_count = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = meshes[i];
            for (u32 j = 0 ; j < mesh.indices.count ; j++) {
                mesh.indices[j] += vertex_count;
            }
            vertex_count += mesh.vertices.count;
            index_count += mesh.indices.count;
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
            auto& mesh = meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertices.size(), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.ibo.id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.indices.size(), mesh.indices.indices);

            vertex_offset += mesh.vertices.size();
            index_offset += mesh.indices.size();
        }

        drawable.type = GL_TRIANGLES;
        drawable.strips = 1;
        drawable.vertices_per_strip = index_count;
    }

    void Model::free() {
        for (auto& mesh : meshes) {
            mesh.free();
        }
        for (auto& material : materials) {
            material.second.free();
        }
    }

    void Model::generate(const std::string &filepath, u32 flags) {
        AssimpCore::read_file([this, &filepath, &flags](const aiScene* scene) {
            std::string directory = filepath.substr(0, filepath.find_last_of('/'));
            parse_meshes(scene->mRootNode, scene, *this, directory, flags);
        }, filepath, flags);
    }

}