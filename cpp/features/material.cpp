#include <features/material.h>

namespace gl {

    void Material::init(
            bool flipUV,
            const char* albedoPath,
            const char* normalPath,
            const char* parallaxPath,
            const char* metallicPath,
            const char* roughnessPath,
            const char* aoPath
    ) {
        ImageParams params;
        params.minFilter = GL_LINEAR_MIPMAP_LINEAR;

        if (albedoPath) {
            Image image = ImageReader::read(albedoPath, flipUV);

            albedo.init();
            albedo.load(image, params);
            enableAlbedo = albedo.id != InvalidImageBuffer;

            image.free();
        }

        if (normalPath) {
            Image image = ImageReader::read(normalPath, flipUV);

            normal.init();
            normal.load(image, params);
            enableNormal = normal.id != InvalidImageBuffer;

            image.free();
        }

        if (parallaxPath) {
            Image image = ImageReader::read(parallaxPath, flipUV);

            parallax.init();
            parallax.load(image, params);
            enableParallax = parallax.id != InvalidImageBuffer;

            image.free();
        }

        if (metallicPath) {
            Image image = ImageReader::read(metallicPath, flipUV);

            metallic.init();
            metallic.load(image, params);
            enableMetallic = metallic.id != InvalidImageBuffer;

            image.free();
        }

        if (roughnessPath) {
            Image image = ImageReader::read(roughnessPath, flipUV);

            roughness.init();
            roughness.load(image, params);
            enableRoughness = roughness.id != InvalidImageBuffer;

            image.free();
        }

        if (aoPath) {
            Image image = ImageReader::read(aoPath, flipUV);

            ao.init();
            ao.load(image, params);
            enableAO = ao.id != InvalidImageBuffer;

            image.free();
        }
    }

    void Material::free() {
        albedo.free();
        normal.free();
        parallax.free();
        metallic.free();
        roughness.free();
        ao.free();
    }

    void Material::free(std::vector<Material>& materials) {
        for (auto& m : materials) {
            m.free();
        }
    }

    void Material::update(Shader& shader, int slot) {
        if (enableAlbedo) {
            shader.bindSamplerStruct("material", "albedo", slot++, albedo);
        }

        if (enableNormal) {
            shader.bindSamplerStruct("material", "normal", slot++, normal);
        }

        if (enableParallax) {
            shader.bindSamplerStruct("material", "parallax", slot++, parallax);
        }

        if (enableMetallic) {
            shader.bindSamplerStruct("material", "metallic", slot++, metallic);
        }

        if (enableRoughness) {
            shader.bindSamplerStruct("material", "roughness", slot++, roughness);
        }

        if (enableAO) {
            shader.bindSamplerStruct("material", "ao", slot++, ao);
        }

        shader.setUniformStructArgs("material", "color", color);
        shader.setUniformStructArgs("material", "enable_albedo", enableAlbedo);

        shader.setUniformStructArgs("material", "enable_normal", enableNormal);

        shader.setUniformStructArgs("material", "height_scale", heightScale);
        shader.setUniformStructArgs("material", "parallax_min_layers", parallaxMinLayers);
        shader.setUniformStructArgs("material", "parallax_max_layers", parallaxMaxLayers);
        shader.setUniformStructArgs("material", "enable_parallax", enableParallax);

        shader.setUniformStructArgs("material", "enable_metallic", enableMetallic);
        shader.setUniformStructArgs("material", "metallic_factor", metallicFactor);

        shader.setUniformStructArgs("material", "enable_roughness", enableRoughness);
        shader.setUniformStructArgs("material", "roughness_factor", roughnessFactor);

        shader.setUniformStructArgs("material", "enable_ao", enableAO);
        shader.setUniformStructArgs("material", "ao_factor", aoFactor);
    }

}