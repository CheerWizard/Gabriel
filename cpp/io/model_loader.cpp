#include <io/model_loader.h>

#include <unordered_map>

namespace gl {

    VertexFormat VertexMesh::format = {
            { Attributes::POS3, Attributes::UV, Attributes::NORMAL, Attributes::TANGENT },
            sizeof(VertexMesh)
    };

    static VertexMesh parseVertex(aiMesh* mesh, u32 i) {
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

    static Mesh parseMesh(aiMesh *mesh)
    {
        Mesh result;
        std::vector<u32> indices;

        result.vertices.init(mesh->mNumVertices);

        for (int i = 0 ; i < mesh->mNumVertices ; i++) {
            result.vertices[i] = parseVertex(mesh, i);
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

    static void parseMeshes(
            aiNode* node, const aiScene* scene,
            Model& model,
            const std::string& directory, u32 flags
    ) {
        auto& meshes = model.meshes;
        auto& materials = model.materials;

        for (u32 i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Mesh result = parseMesh(mesh);
            result.materialIndex = MaterialLoader::parseMaterial(materials, scene, mesh, directory, flags);
            meshes.push_back(result);
        }

        for (u32 i = 0 ; i < node->mNumChildren ; i++)
        {
            parseMeshes(node->mChildren[i], scene, model, directory, flags);
        }
    }

    void Model::init(DrawableElements& drawable) {
        // invalidate model vertices and indices to group together
        size_t meshCount = meshes.size();
        u32 vertexCount = 0;
        u32 indexCount = 0;
        for (u32 i = 0 ; i < meshCount ; i++) {
            auto& mesh = meshes[i];
            for (u32 j = 0 ; j < mesh.indices.count ; j++) {
                mesh.indices[j] += vertexCount;
            }
            vertexCount += mesh.vertices.count;
            indexCount += mesh.indices.count;
        }

        drawable.vao.init();
        drawable.vao.bind();

        glGenBuffers(1, &drawable.vbo.id);
        glBindBuffer(GL_ARRAY_BUFFER, drawable.vbo.id);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(VertexMesh), null, GL_DYNAMIC_DRAW);
        drawable.vbo.setFormat(VertexMesh::format);

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

    void Model::free() {
        for (auto& mesh : meshes) {
            mesh.free();
        }
        for (auto& material : materials) {
            material.second.free();
        }
    }

    void Model::generate(const std::string &filepath, u32 flags) {
        AssimpCore::readFile([this, &filepath, &flags](const aiScene* scene) {
            std::string directory = filepath.substr(0, filepath.find_last_of('/'));
            parseMeshes(scene->mRootNode, scene, *this, directory, flags);
        }, filepath, flags);
    }

}