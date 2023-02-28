#include <material.h>
#include <texture.h>

namespace gl {

    material material_init(
            const char* diffuse_path,
            const char* specular_path,
            const char* skybox_path,
            const char* normal_path,
            const char* parallax_path
    ) {
        material new_material;
        if (diffuse_path) {
            gl::texture2d_init(new_material.diffuse, diffuse_path);
            new_material.enable_diffuse = new_material.diffuse.id != invalid_texture;
        }
        if (specular_path) {
            gl::texture2d_init(new_material.specular, specular_path);
            new_material.enable_specular = new_material.specular.id != invalid_texture;
        }
        if (skybox_path) {
            gl::texture2d_init(new_material.skybox, skybox_path);
            new_material.enable_skybox = new_material.skybox.id != invalid_texture;
        }
        if (normal_path) {
            gl::texture2d_init(new_material.normal, normal_path);
            new_material.enable_normal = new_material.normal.id != invalid_texture;
        }
        if (parallax_path) {
            gl::texture2d_init(new_material.parallax, parallax_path);
            new_material.enable_parallax = new_material.parallax.id != invalid_texture;
        }
        return new_material;
    }

    void material_update_textures(u32 shader, gl::material& material) {
        if (material.enable_diffuse) {
            shader_set_uniform_struct(shader, "material", material.diffuse.sampler);
            glActiveTexture(GL_TEXTURE0 + material.diffuse.sampler.value);
            glBindTexture(material.diffuse.type, material.diffuse.id);
        }

        if (material.enable_specular) {
            shader_set_uniform_struct(shader, "material", material.specular.sampler);
            glActiveTexture(GL_TEXTURE0 + material.specular.sampler.value);
            glBindTexture(material.specular.type, material.specular.id);
        }

        if (material.enable_skybox) {
            shader_set_uniform_struct(shader, "material", material.skybox.sampler);
            glActiveTexture(GL_TEXTURE0 + material.skybox.sampler.value);
            glBindTexture(material.skybox.type, material.skybox.id);
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
    }

    void material_update(u32 shader, gl::material& material) {
        material_update_textures(shader, material);
        shader_set_uniform_struct4v(shader, "material", "color", material.color);
        shader_set_uniform_structf(shader, "material", "diffuse_factor", material.diffuse_factor);
        shader_set_uniform_structb(shader, "material", "enable_diffuse", material.enable_diffuse);
        shader_set_uniform_structf(shader, "material", "specular_factor", material.specular_factor);
        shader_set_uniform_structb(shader, "material", "enable_specular", material.enable_specular);
        shader_set_uniform_structf(shader, "material", "shininess", material.shininess);
        shader_set_uniform_structb(shader, "material", "enable_skybox", material.enable_skybox);
        shader_set_uniform_structf(shader, "material", "reflection", material.reflection);
        shader_set_uniform_structf(shader, "material", "refraction", material.refraction);
        shader_set_uniform_structb(shader, "material", "enable_normal", material.enable_normal);
        shader_set_uniform_structf(shader, "material", "height_scale", material.height_scale);
        shader_set_uniform_structf(shader, "material", "parallax_min_layers", material.parallax_min_layers);
        shader_set_uniform_structf(shader, "material", "parallax_max_layers", material.parallax_max_layers);
        shader_set_uniform_structb(shader, "material", "enable_parallax", material.enable_parallax);
    }

    void material_free(material& m) {
        texture_free(m.diffuse.id);
        texture_free(m.specular.id);
        texture_free(m.skybox.id);
        texture_free(m.normal.id);
        texture_free(m.parallax.id);
    }

}