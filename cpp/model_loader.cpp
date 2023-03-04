#include <model_loader.h>
#include <geometry.h>
#include <texture.h>

#include <sstream>
#include <unordered_map>

namespace io {

    using namespace gl;

    VertexFormat VertexMesh::format = {
            { vec3, vec2, vec3, vec3 },
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

    static std::unordered_map<std::string, Texture> textures_table {};

    static void read_material(
            aiMaterial* material,
            aiTextureType type,
            const std::string& directory,
            u32 flags,
            Texture& texture,
            const TextureParams& params = {}
    ) {
        aiString texture_file;
        material->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);
        std::stringstream ss;
        ss << directory << "/" << texture_file.data;
        std::string texture_filepath = ss.str();

        if (textures_table.find(texture_filepath) != textures_table.end()) {
            texture = textures_table[texture_filepath];
            return;
        }

        texture.init(texture_filepath.c_str(), flags & aiProcess_FlipUVs, params);
        if (texture.id != invalid_texture) {
            textures_table[texture_filepath] = texture;
        }
    }

    static Mesh parse_mesh(aiMesh *mesh, const aiScene *scene, const std::string& directory, u32 flags)
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

    static void parse_node(
            aiNode *node, const aiScene *scene,
            std::vector<Mesh>& meshes,
            std::unordered_map<u32, Material>& materials,
            const std::string& directory, u32 flags
    ) {
        for (u32 i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

            io::Mesh result = parse_mesh(mesh, scene, directory, flags);

            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                result.material_index = mesh->mMaterialIndex;

                gl::Material result_material;
                gl::TextureParams material_params;
                material_params.min_filter = GL_LINEAR_MIPMAP_LINEAR;

                read_material(material, aiTextureType_BASE_COLOR, directory, flags, result_material.albedo, material_params);
                result_material.enable_albedo = result_material.albedo.id != invalid_texture;

                read_material(material, aiTextureType_NORMALS, directory, flags, result_material.normal, material_params);
                result_material.enable_normal = result_material.normal.id != invalid_texture;

                read_material(material, aiTextureType_DISPLACEMENT, directory, flags, result_material.parallax, material_params);
                result_material.enable_parallax = result_material.parallax.id != invalid_texture;

                read_material(material, aiTextureType_METALNESS, directory, flags, result_material.metallic, material_params);
                result_material.enable_metallic = result_material.metallic.id != invalid_texture;

                read_material(material, aiTextureType_DIFFUSE_ROUGHNESS, directory, flags, result_material.roughness, material_params);
                result_material.enable_roughness = result_material.roughness.id != invalid_texture;

                read_material(material, aiTextureType_AMBIENT_OCCLUSION, directory, flags, result_material.ao, material_params);
                result_material.enable_ao = result_material.ao.id != invalid_texture;

                materials[result.material_index] = result_material;
            }

            meshes.push_back(result);
        }

        for (u32 i = 0 ; i < node->mNumChildren ; i++)
        {
            parse_node(node->mChildren[i], scene, meshes, materials, directory, flags);
        }
    }

    static Model model_read(
            const std::string& filepath,
            u32 flags
    ) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            print_err("model_read(): failed to read model " << filepath);
            return {};
        }

        std::vector<Mesh> meshes;
        std::unordered_map<u32, Material> materials;
        std::string directory = filepath.substr(0, filepath.find_last_of('/'));
        parse_node(scene->mRootNode, scene, meshes, materials, directory, flags);

        return { meshes, materials };
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

        elements.vao.init();
        elements.vao.bind();

        glGenBuffers(1, &elements.vbo.id);
        glBindBuffer(GL_ARRAY_BUFFER, elements.vbo.id);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(VertexMesh), null, GL_DYNAMIC_DRAW);
        elements.vbo.set_format(VertexMesh::format);

        glGenBuffers(1, &elements.ibo.id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements.ibo.id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertex_offset = 0;
        u32 index_offset = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = model.meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, elements.vbo.id);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertex_count * sizeof(VertexMesh), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements.ibo.id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.index_count * sizeof(u32), mesh.indices);

            vertex_offset += mesh.vertex_count * sizeof(VertexMesh);
            index_offset += mesh.index_count * sizeof(u32);
        }

        elements.index_count = index_count;
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

        elements.free();
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