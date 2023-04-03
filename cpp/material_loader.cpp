#include <material_loader.h>

namespace io {

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

    u32 parse_material(
            std::unordered_map<u32, Material>& materials,
            const aiScene* scene, aiMesh* mesh,
            const std::string& directory, u32 flags
    ) {
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            u32 material_index = mesh->mMaterialIndex;

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

            materials[material_index] = result_material;
            return material_index;
        }
        return 0;
    }

}