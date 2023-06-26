#include <features/material.h>
#include <io/image_loader.h>

namespace gl {

    void Material::load(
            const bool flipUV,
            const char* albedoPath,
            const char* normalPath,
            const char* parallaxPath,
            const char* metallicPath,
            const char* roughnessPath,
            const char* aoPath,
            const char* emissionPath
    ) {
        ImageParams params;
        params.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        load(albedoPath, flipUV, params, albedo, enableAlbedo);
        load(normalPath, flipUV, params, normal, enableNormal);
        load(parallaxPath, flipUV, params, parallax, enableParallax);
        load(metallicPath, flipUV, params, metallic, enableMetallic);
        load(roughnessPath, flipUV, params, roughness, enableRoughness);
        load(aoPath, flipUV, params, ao, enableAO);
        load(emissionPath, flipUV, params, emission, enableEmission);
    }

    void Material::free() {
        albedo.free();
        normal.free();
        parallax.free();
        metallic.free();
        roughness.free();
        ao.free();
        emission.free();
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

        if (enableEmission) {
            shader.bindSamplerStruct("material", "emission", slot++, emission);
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

        shader.setUniformStructArgs("material", "enable_emission", enableEmission);
        shader.setUniformStructArgs("material", "emission_factor", emissionFactor);
    }

    void Material::load(const char* filepath, const bool uv, const ImageParams& params, ImageBuffer &outBuffer, bool &outEnable) {
        if (filepath) {
            Image image = ImageReader::read(filepath, uv);

            outBuffer.init();
            outBuffer.load(image, params);
            outEnable = outBuffer.id != InvalidImageBuffer;

            image.free();
        }
    }

}