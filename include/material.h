#pragma once

#include <shader.h>

namespace gl {

    struct texture final {
        u32 id;
        uniform_i sampler;
    };

    struct material final {
        texture diffuse = { 0, { "diffuse", 0 } };
        texture specular = { 0, { "specular", 1 } };
        texture emission = { 0, { "emission", 2 } };
        float shininess = 32;
    };

    material material_init(
            const char* diffuse_path,
            const char* specular_path,
            const char* emission_path
    );
    void material_update(u32 shader, material& material);

}
