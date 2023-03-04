#pragma once

#include <primitives.h>
#include <shader.h>
#include <file_utils.h>

#include <array>

#include <glad/glad.h>

namespace gl {

#define invalid_texture 0

    struct TextureData final {
        int width, height, internal_format, data_format;
        u32 primitive_type = GL_UNSIGNED_BYTE;
        void* data = null;
        int samples = 1;
    };

    struct TextureParams final {
        int s = GL_REPEAT;
        int t = GL_REPEAT;
        int r = GL_REPEAT;
        glm::vec4 border_color = { 0, 0, 0, 1 };
        int min_filter = GL_LINEAR;
        int mag_filter = GL_LINEAR;
        bool srgb = false;
        float lod_bias = -0.4f;
    };

    struct TextureFace final {
        const char* filepath;
        int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        bool flip_uv = false;
        bool srgb = false;
    };

    struct Texture final {
        u32 id = invalid_texture;
        u32 type = GL_TEXTURE_2D;
        UniformI sampler = { "sampler", 0 };

        void init(
                const char* filepath,
                bool flip_uv = false,
                const TextureParams& params = {}
        );

        void init(
                const gl::TextureData& texture_data,
                const TextureParams& params = {}
        );

        void init_hdr(
                const char* filepath,
                bool flip_uv = false
        );

        void init_cubemap(
                const std::array<TextureFace, 6>& faces,
                const TextureParams& params = {}
        );

        void init_cubemap(
                const TextureData& data,
                const TextureParams& params = {}
        );

        void free();
        void free(int size);

        void bind() const;
        static void unbind();

        void update(Shader& shader);
        void bind_params(const TextureParams& params);
        void update_params(const TextureParams& params);

        void generate_mipmaps(const TextureParams& params);
    };

}