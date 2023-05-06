#include <io/material_loader.h>

namespace gl {

    static std::unordered_map<std::string, ImageBuffer> bufferTable {};

    static void readMaterial(
            aiMaterial* material,
            aiTextureType type,
            const std::string& directory,
            u32 flags,
            ImageBuffer& buffer,
            const ImageParams& params = {}
    ) {
        aiString textureFile;
        material->Get(AI_MATKEY_TEXTURE(type, 0), textureFile);
        std::stringstream ss;
        ss << directory << "/" << textureFile.data;
        std::string imageFilepath = ss.str();

        if (bufferTable.find(imageFilepath) != bufferTable.end()) {
            buffer = bufferTable[imageFilepath];
            return;
        }

        Image image = ImageReader::read(imageFilepath.c_str(), flags & aiProcess_FlipUVs);

        buffer.init();
        buffer.load(image, params);
        if (buffer.id != InvalidImageBuffer) {
            bufferTable[imageFilepath] = buffer;
        }

        image.free();
    }

    u32 MaterialLoader::parseMaterial(
            std::unordered_map<u32, Material>& materials,
            const aiScene* scene, aiMesh* mesh,
            const std::string& directory, u32 flags
    ) {
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            u32 materialIndex = mesh->mMaterialIndex;

            Material resultMaterial;
            ImageParams materialParams;
            materialParams.minFilter = GL_LINEAR_MIPMAP_LINEAR;

            readMaterial(material, aiTextureType_BASE_COLOR, directory, flags, resultMaterial.albedo, materialParams);
            resultMaterial.enableAlbedo = resultMaterial.albedo.id != InvalidImageBuffer;

            readMaterial(material, aiTextureType_NORMALS, directory, flags, resultMaterial.normal, materialParams);
            resultMaterial.enableNormal = resultMaterial.normal.id != InvalidImageBuffer;

            readMaterial(material, aiTextureType_DISPLACEMENT, directory, flags, resultMaterial.parallax,
                         materialParams);
            resultMaterial.enableParallax = resultMaterial.parallax.id != InvalidImageBuffer;

            readMaterial(material, aiTextureType_METALNESS, directory, flags, resultMaterial.metallic, materialParams);
            resultMaterial.enableMetallic = resultMaterial.metallic.id != InvalidImageBuffer;

            readMaterial(material, aiTextureType_DIFFUSE_ROUGHNESS, directory, flags, resultMaterial.roughness,
                         materialParams);
            resultMaterial.enableRoughness = resultMaterial.roughness.id != InvalidImageBuffer;

            readMaterial(material, aiTextureType_AMBIENT_OCCLUSION, directory, flags, resultMaterial.ao, materialParams);
            resultMaterial.enableAO = resultMaterial.ao.id != InvalidImageBuffer;

            materials[materialIndex] = resultMaterial;
            return materialIndex;
        }
        return 0;
    }

}