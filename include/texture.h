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
        int width, height, data_format, internal_format;
        u32 primitive_type = GL_UNSIGNED_BYTE;
        void* data;
    };

    struct texture_2d_params final {
        int s = GL_REPEAT;
        int t = GL_REPEAT;
        glm::vec4 border_color = { 0, 0, 0, 1 };
        int min_filter = GL_LINEAR_MIPMAP_LINEAR;
        int mag_filter = GL_LINEAR;
        bool generate_mipmap = true;
        bool srgb = false;
    };

    struct texture_3d_params final {
        int s = GL_CLAMP_TO_EDGE;
        int t = GL_CLAMP_TO_EDGE;
        int r = GL_CLAMP_TO_EDGE;
        glm::vec4 border_color = { 0, 0, 0, 1 };
        int min_filter = GL_LINEAR;
        int mag_filter = GL_LINEAR;
    };

    void texture2d_init(
            texture& texture,
            const char* filepath,
            bool flip_uv = false,
            const texture_2d_params& params = {}
    );
    void texture2d_init(
            texture& texture,
            const gl::texture_data& texture_data,
            const texture_2d_params& params = {}
    );

    void texture_free(u32 tbo);
    void textures_free(u32* tbo, int size);

    void texture_update(u32 shader, texture& texture);
    void texture_update(const texture& texture, const texture_2d_params& params);
    void texture_update(const texture& texture, const texture_3d_params& params);

    struct texture_face final {
        const char* filepath;
        int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        bool flip_uv = false;
        bool srgb = false;
    };

    void texture_cube_init(
        texture& texture,
        const std::array<texture_face, 6>& faces,
        const texture_3d_params& params = {}
    );

}