#include <model_loader.h>
#include <geometry.h>
#include <texture.h>

#include <sstream>
#include <unordered_map>

namespace io {

    using namespace gl;

    vertex_format vertex_mesh::format = {
            { vec3, vec2, vec3, vec3 },
            sizeof(vertex_mesh)
    };

    static vertex_mesh parse_vertex(aiMesh* mesh, u32 i) {
        vertex_mesh vertex;

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

    static std::unordered_map<std::string, texture> s_material_cache {};

    static void read_material(
            aiMaterial* material,
            aiTextureType type,
            const std::string& directory,
            u32 flags,
            texture& texture,
            const texture_params& params = {}
    ) {
        aiString texture_file;
        material->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);
        std::stringstream ss;
        ss << directory << "/" << texture_file.data;
        std::string texture_filepath = ss.str();

        if (s_material_cache.find(texture_filepath) != s_material_cache.end()) {
            texture = s_material_cache[texture_filepath];
            return;
        }

        gl::texture_init(texture, texture_filepath.c_str(), flags & aiProcess_FlipUVs, params);
        if (texture.id != invalid_texture) {
            s_material_cache[texture_filepath] = texture;
        }
    }

    static mesh parse_mesh(aiMesh *mesh, const aiScene *scene, const std::string& directory, u32 flags)
    {
        io::mesh result;
        std::vector<u32> indices;

        result.vertex_count = mesh->mNumVertices;
        result.vertices.data = new vertex_mesh[mesh->mNumVertices];

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
            std::vector<mesh>& meshes,
            std::unordered_map<u32, material>& materials,
            const std::string& directory, u32 flags
    ) {
        for (u32 i = 0 ; i < node->mNumMeshes ; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

            io::mesh result = parse_mesh(mesh, scene, directory, flags);

            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                result.material_index = mesh->mMaterialIndex;

                gl::material result_material;
                gl::texture_params material_params;
                material_params.generate_mipmap = true;
                material_params.min_filter = GL_LINEAR_MIPMAP_LINEAR;

                material_params.srgb = false;
                read_material(material, aiTextureType_BASE_COLOR, directory, flags, result_material.albedo, material_params);
                result_material.enable_albedo = result_material.albedo.id != invalid_texture;

                material_params.srgb = false;
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

    static model model_read(
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

        std::vector<mesh> meshes;
        std::unordered_map<u32, material> materials;
        std::string directory = filepath.substr(0, filepath.find_last_of('/'));
        parse_node(scene->mRootNode, scene, meshes, materials, directory, flags);

        return { meshes, materials };
    }

    drawable_model model_init(const std::string& filepath, u32 flags) {
        drawable_model drawable;
        drawable.model = model_read(filepath, flags);
        // invalidate model vertices and indices to group together
        size_t mesh_count = drawable.model.meshes.size();
        u32 vertex_count = 0;
        u32 index_count = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = drawable.model.meshes[i];
            for (u32 j = 0 ; j < mesh.index_count ; j++) {
                mesh.indices[j] += vertex_count;
            }
            vertex_count += mesh.vertex_count;
            index_count += mesh.index_count;
        }

        drawable.elements.vao = vao_init();
        vao_bind(drawable.elements.vao);

        glGenBuffers(1, &drawable.elements.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, drawable.elements.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(vertex_mesh), null, GL_DYNAMIC_DRAW);
        vbo_set_format(vertex_mesh::format);

        glGenBuffers(1, &drawable.elements.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.elements.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertex_offset = 0;
        u32 index_offset = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = drawable.model.meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, drawable.elements.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertex_count * sizeof(vertex_mesh), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.elements.ibo);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.index_count * sizeof(u32), mesh.indices);

            vertex_offset += mesh.vertex_count * sizeof(vertex_mesh);
            index_offset += mesh.index_count * sizeof(u32);
        }

        drawable.elements.index_count = index_count;

        return drawable;
    }

    drawable_models models_init(const std::string& filepath, u32 flags) {
        drawable_models drawables;
        drawables.model = model_read(filepath, flags);

        size_t mesh_size = drawables.model.meshes.size();
        drawables.elements.resize(mesh_size);
        for (int i = 0 ; i < drawables.elements.size() ; i++) {
            auto& element = drawables.elements[i];
            auto& mesh = drawables.model.meshes[i];

            element.vao = vao_init();
            vao_bind(element.vao);

            glGenBuffers(1, &element.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, element.vbo);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(vertex_mesh), mesh.vertices.to_float(), GL_DYNAMIC_DRAW);
            vbo_set_format(vertex_mesh::format);

            glGenBuffers(1, &element.ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element.ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(u32), mesh.indices, GL_DYNAMIC_DRAW);

            element.index_count = mesh.index_count;
        }

        return drawables;
    }

    void model_free(drawable_model& drawable) {
        for (auto& mesh : drawable.model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }

        for (auto& material : drawable.model.materials) {
            gl::material_free(material.second);
        }

        gl::drawable_free(drawable.elements);
    }

    void models_free(drawable_models& drawables) {
        for (auto& mesh : drawables.model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }

        for (auto& material : drawables.model.materials) {
            gl::material_free(material.second);
        }

        for (auto& element : drawables.elements) {
            gl::drawable_free(element);
        }
    }

}