#include <material.h>
#include <texture.h>

namespace gl {

    material material_init(
            bool flip_uv,
            const char* albedo_path,
            const char* normal_path,
            const char* parallax_path,
            const char* metallic_path,
            const char* roughness_path,
            const char* ao_path
    ) {
        material new_material;
        if (albedo_path) {
            gl::texture_2d_params params;
            params.srgb = true;
            gl::texture2d_init(new_material.albedo, albedo_path, flip_uv, params);
            new_material.enable_albedo = new_material.albedo.id != invalid_texture;
        }
        if (normal_path) {
            gl::texture2d_init(new_material.normal, normal_path, flip_uv);
            new_material.enable_normal = new_material.normal.id != invalid_texture;
        }
        if (parallax_path) {
            gl::texture2d_init(new_material.parallax, parallax_path, flip_uv);
            new_material.enable_parallax = new_material.parallax.id != invalid_texture;
        }
        if (metallic_path) {
            gl::texture2d_init(new_material.metallic, metallic_path, flip_uv);
            new_material.enable_metallic = new_material.metallic.id != invalid_texture;
        }
        if (roughness_path) {
            gl::texture2d_init(new_material.roughness, roughness_path, flip_uv);
            new_material.enable_roughness = new_material.roughness.id != invalid_texture;
        }
        if (ao_path) {
            gl::texture2d_init(new_material.ao, ao_path, flip_uv);
            new_material.enable_ao = new_material.ao.id != invalid_texture;
        }
        return new_material;
    }

    void material_update_textures(u32 shader, gl::material& material) {
        if (material.enable_albedo) {
            shader_set_uniform_struct(shader, "material", material.albedo.sampler);
            glActiveTexture(GL_TEXTURE0 + material.albedo.sampler.value);
            glBindTexture(material.albedo.type, material.albedo.id);
        }

        if (material.enable_normal) {
            shader_set_uniform_struct(shader, "material", material.normal.sampler);
            glActiveTexture(GL_TEXTURE0 + material.normal.sampler.value);
            glBindTexture(material.normal.type, material.normal.id);
        }

        if (material.enable_parallax) {
            shader_set_uniform_struct(shader, "material", material.parallax.sampler);
            glActiveTexture(GL_TEXTURE0 + material.parallax.sampler.value);
            glBindTexture(material.parallax.type, material.parallax.id);
        }

        if (material.enable_metallic) {
            shader_set_uniform_struct(shader, "material", material.metallic.sampler);
            glActiveTexture(GL_TEXTURE0 + material.metallic.sampler.value);
            glBindTexture(material.metallic.type, material.metallic.id);
        }

        if (material.enable_roughness) {
            shader_set_uniform_struct(shader, "material", material.roughness.sampler);
            glActiveTexture(GL_TEXTURE0 + material.roughness.sampler.value);
            glBindTexture(material.roughness.type, material.roughness.id);
        }

        if (material.enable_ao) {
            shader_set_uniform_struct(shader, "material", material.ao.sampler);
            glActiveTexture(GL_TEXTURE0 + material.ao.sampler.value);
            glBindTexture(material.ao.type, material.ao.id);
        }

        if (material.enable_env) {
            shader_set_uniform_struct(shader, "material", material.env.sampler);
            glActiveTexture(GL_TEXTURE0 + material.env.sampler.value);
            glBindTexture(material.env.type, material.env.id);
        }
    }

    void material_update(u32 shader, gl::material& material) {
        material_update_textures(shader, material);

        shader_set_uniform_struct4v(shader, "material", "color", material.color);
        shader_set_uniform_structb(shader, "material", "enable_albedo", material.enable_albedo);

        shader_set_uniform_structb(shader, "material", "enable_normal", material.enable_normal);

        shader_set_uniform_structf(shader, "material", "height_scale", material.height_scale);
        shader_set_uniform_structf(shader, "material", "parallax_min_layers", material.parallax_min_layers);
        shader_set_uniform_structf(shader, "material", "parallax_max_layers", material.parallax_max_layers);
        shader_set_uniform_structb(shader, "material", "enable_parallax", material.enable_parallax);

        shader_set_uniform_structb(shader, "material", "enable_metallic", material.enable_metallic);
        shader_set_uniform_structf(shader, "material", "metallic_factor", material.metallic_factor);

        shader_set_uniform_structb(shader, "material", "enable_roughness", material.enable_roughness);
        shader_set_uniform_structf(shader, "material", "roughness_factor", material.roughness_factor);

        shader_set_uniform_structb(shader, "material", "enable_ao", material.enable_ao);
        shader_set_uniform_structf(shader, "material", "ao_factor", material.ao_factor);

        shader_set_uniform_structb(shader, "material", "enable_env", material.enable_env);
        shader_set_uniform_structf(shader, "material", "reflection", material.reflection);
        shader_set_uniform_structf(shader, "material", "refraction", material.refraction);
    }

    void material_free(material& m) {
        texture_free(m.albedo.id);
        texture_free(m.normal.id);
        texture_free(m.parallax.id);
        texture_free(m.metallic.id);
        texture_free(m.roughness.id);
        texture_free(m.ao.id);
        texture_free(m.env.id);
    }

}