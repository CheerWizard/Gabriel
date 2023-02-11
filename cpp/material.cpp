#include <material.h>
#include <texture.h>

namespace gl {

    material material_init(
            const char* diffuse_path,
            const char* specular_path,
            const char* emission_path
    ) {
        material new_material;
        new_material.diffuse.id = gl::texture2d_init(diffuse_path);
        new_material.specular.id = gl::texture2d_init(specular_path);
        new_material.emission.id = gl::texture2d_init(emission_path);
        return new_material;
    }

    void material_update(u32 shader, material& material) {
        shader_set_uniform_struct(shader, "material", material.diffuse.sampler);
        shader_set_uniform_struct(shader, "material", material.specular.sampler);
        shader_set_uniform_struct(shader, "material", material.emission.sampler);
        shader_set_uniform_structf(shader, "material", "shininess", material.shininess);
        texture_bind(material.diffuse.id, GL_TEXTURE_2D, material.diffuse.sampler.value);
        texture_bind(material.specular.id, GL_TEXTURE_2D, material.specular.sampler.value);
        texture_bind(material.emission.id, GL_TEXTURE_2D, material.emission.sampler.value);
    }

}