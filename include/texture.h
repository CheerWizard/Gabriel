#pragma once

#include <primitives.h>
#include <shader.h>
#include <file_utils.h>

#include <array>

#include <glad/glad.h>

namespace gl {

#define invalid_texture 0

    struct texture final {
        u32 id = invalid_texture;
        u32 type = GL_TEXTURE_2D;
        uniform_i sampler = { "sampler", 0 };
    };

    struct texture_data final {
        int width, height, internal_format, data_format;
        u32 primitive_type = GL_UNSIGNED_BYTE;
        void* data = null;
        int samples = 1;
    };

    struct texture_params final {
        int s = GL_REPEAT;
        int t = GL_REPEAT;
        int r = GL_REPEAT;
        glm::vec4 border_color = { 0, 0, 0, 1 };
        int min_filter = GL_LINEAR;
        int mag_filter = GL_LINEAR;
        bool generate_mipmap = false;
        bool srgb = false;
        float lod_bias = -0.4f;
    };

    struct texture_face final {
        const char* filepath;
        int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        bool flip_uv = false;
        bool srgb = false;
    };

    void texture_init(
            texture& texture,
            const char* filepath,
            bool flip_uv = false,
            const texture_params& params = {}
    );

    void texture_init(
            texture& texture,
            const gl::texture_data& texture_data,
            const texture_params& params = {}
    );

    void texture_hdr_init(
            texture& texture,
            const char* filepath,
            bool flip_uv = false
    );

    void texture_cube_init(
            texture& texture,
            const std::array<texture_face, 6>& faces,
            const texture_params& params = {}
    );

    void texture_cube_init(
            texture& texture,
            const texture_data& data,
            const texture_params& params = {}
    );

    void texture_free(u32 tbo);
    void textures_free(u32* tbo, int size);

    void texture_unbind();

    void texture_update(u32 shader, texture& texture);
    void texture_params_bind(const texture& texture, const texture_params& params);
    void texture_params_update(const texture& texture, const texture_params& params);

    void texture_generate_mipmaps(const gl::texture& texture);

}