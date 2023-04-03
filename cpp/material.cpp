#include <material.h>

namespace gl {

    void Material::init(
            bool flip_uv,
            const char* albedo_path,
            const char* normal_path,
            const char* parallax_path,
            const char* metallic_path,
            const char* roughness_path,
            const char* ao_path
    ) {
        gl::TextureParams params;
        params.min_filter = GL_LINEAR_MIPMAP_LINEAR;

        if (albedo_path) {
            albedo.init(albedo_path, flip_uv, params);
            enable_albedo = albedo.id != invalid_texture;
        }

        if (normal_path) {
            normal.init(normal_path, flip_uv, params);
            enable_normal = normal.id != invalid_texture;
        }

        if (parallax_path) {
            parallax.init(parallax_path, flip_uv, params);
            enable_parallax = parallax.id != invalid_texture;
        }

        if (metallic_path) {
            metallic.init(metallic_path, flip_uv, params);
            enable_metallic = metallic.id != invalid_texture;
        }

        if (roughness_path) {
            roughness.init(roughness_path, flip_uv, params);
            enable_roughness = roughness.id != invalid_texture;
        }

        if (ao_path) {
            ao.init(ao_path, flip_uv, params);
            enable_ao = ao.id != invalid_texture;
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
        if (enable_albedo) {
            shader.bind_sampler_struct("material", "albedo", slot++, albedo);
        }

        if (enable_normal) {
            shader.bind_sampler_struct("material", "normal", slot++, normal);
        }

        if (enable_parallax) {
            shader.bind_sampler_struct("material", "parallax", slot++, parallax);
        }

        if (enable_metallic) {
            shader.bind_sampler_struct("material", "metallic", slot++, metallic);
        }

        if (enable_roughness) {
            shader.bind_sampler_struct("material", "roughness", slot++, roughness);
        }

        if (enable_ao) {
            shader.bind_sampler_struct("material", "ao", slot++, ao);
        }

        shader.set_uniform_struct_args("material", "color", color);
        shader.set_uniform_struct_args("material", "enable_albedo", enable_albedo);

        shader.set_uniform_struct_args("material", "enable_normal", enable_normal);

        shader.set_uniform_struct_args("material", "height_scale", height_scale);
        shader.set_uniform_struct_args("material", "parallax_min_layers", parallax_min_layers);
        shader.set_uniform_struct_args("material", "parallax_max_layers", parallax_max_layers);
        shader.set_uniform_struct_args("material", "enable_parallax", enable_parallax);

        shader.set_uniform_struct_args("material", "enable_metallic", enable_metallic);
        shader.set_uniform_struct_args("material", "metallic_factor", metallic_factor);

        shader.set_uniform_struct_args("material", "enable_roughness", enable_roughness);
        shader.set_uniform_struct_args("material", "roughness_factor", roughness_factor);

        shader.set_uniform_struct_args("material", "enable_ao", enable_ao);
        shader.set_uniform_struct_args("material", "ao_factor", ao_factor);
    }

}