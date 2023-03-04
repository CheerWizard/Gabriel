#include <texture.h>
#include <device.h>

namespace gl {

    static int data_formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int internal_formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int internal_formats_srgb[2] = { GL_SRGB, GL_SRGB_ALPHA };

    void Texture::init(
            const char* filepath,
            bool flip_uv,
            const TextureParams& params
    ) {
        io::ImageData image = io::read_image(filepath, flip_uv);
        if (!image.data)
            return;

        gl::TextureData texture_data;
        texture_data.width = image.width;
        texture_data.height = image.height;
        texture_data.data = image.data;
        texture_data.data_format = data_formats[image.channels - 1];
        if (params.srgb) {
            texture_data.internal_format = internal_formats_srgb[image.channels - 3];
        } else {
            texture_data.internal_format = internal_formats[image.channels - 1];
        }

        init(texture_data, params);

        io::free(image.data);
    }

    void Texture::init(
            const TextureData& texture_data,
            const TextureParams& params
    ) {
        u32 texture_type = GL_TEXTURE_2D;
        type = texture_type;
        glGenTextures(1, &id);
        glBindTexture(texture_type, id);

        int width = texture_data.width;
        int height = texture_data.height;
        int data_format = texture_data.data_format;
        int internal_format = texture_data.internal_format;
        u32 primitive_type = texture_data.primitive_type;
        void* data = texture_data.data;

        glTexImage2D(texture_type, 0, internal_format, width, height, 0, data_format, primitive_type, data);

        if (params.min_filter == GL_NEAREST_MIPMAP_NEAREST
            || params.min_filter == GL_NEAREST_MIPMAP_LINEAR
            || params.min_filter == GL_LINEAR_MIPMAP_NEAREST
            || params.min_filter == GL_LINEAR_MIPMAP_LINEAR
        ) {
            generate_mipmaps(params);
        }

        update_params(params);
    }

    void Texture::init_hdr(
            const char* filepath,
            bool flip_uv
    ) {
        io::HDRImageData image = io::read_hdr_image(filepath, flip_uv);
        if (!image.data)
            return;

        gl::TextureData texture_data = { image.width, image.height, GL_RGB16F, GL_RGB, GL_FLOAT, image.data };

        TextureParams params;
        params.min_filter = GL_LINEAR;
        params.mag_filter = GL_LINEAR;
        params.s = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;

        init(texture_data, params);

        io::free(image.data);
    }

    void Texture::free() {
        glDeleteTextures(1, &id);
    }

    void Texture::free(int size) {
        glDeleteTextures(size, &id);
    }

    void Texture::bind() const {
        glActiveTexture(GL_TEXTURE0 + sampler.value);
        glBindTexture(type, id);
    }

    void Texture::unbind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::update(Shader& shader) {
        shader.set_uniform(sampler);
        glActiveTexture(GL_TEXTURE0 + sampler.value);
        glBindTexture(type, id);
    }

    void Texture::generate_mipmaps(const TextureParams& params) {
        u32 texture_type = type;
        glBindTexture(texture_type, id);
        glGenerateMipmap(texture_type);
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameterf(texture_type, GL_TEXTURE_LOD_BIAS, params.lod_bias);
        glTexParameterf(texture_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, device::max_anisotropy_samples);
    }

    void Texture::update_params(const TextureParams &params) {
        u32 texture_type = type;
        glTexParameterfv(texture_type, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.border_color));
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, params.r);
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    }

    void Texture::bind_params(const TextureParams &params) {
        glBindTexture(type, id);
        update_params(params);
    }

    void Texture::init_cubemap(const std::array<TextureFace, 6>& faces, const TextureParams &params) {
        type = GL_TEXTURE_CUBE_MAP;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        for (const auto& face : faces) {
            io::ImageData texture_data = io::read_image(face.filepath, face.flip_uv);
            if (!texture_data.data)
                break;

            int width = texture_data.width;
            int height = texture_data.height;
            int channels = texture_data.channels;
            void* data = texture_data.data;

            int data_format;
            int internal_format;
            switch (channels) {
                case 1:
                    data_format = GL_RED;
                    internal_format = GL_RED;
                    break;
                case 2:
                    data_format = GL_RG;
                    internal_format = GL_RG;
                    break;
                case 4:
                    data_format = GL_RGBA;
                    internal_format = GL_RGBA;
                    break;
                default:
                    data_format = GL_RGB;
                    internal_format = GL_RGB;
                    break;
            }

            if (face.srgb) {
                internal_format = channels == 4 ? GL_SRGB_ALPHA : GL_SRGB;
            }

            glTexImage2D(face.face, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);

            io::free(data);
        }

        update_params(params);
    }

    void Texture::init_cubemap(const TextureData& data, const TextureParams &params) {
        type = GL_TEXTURE_CUBE_MAP;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        for (int i = 0 ; i < 6 ; i++) {
            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    data.internal_format,
                    data.width, data.height, 0,
                    data.data_format,
                    data.primitive_type,
                    data.data
            );
        }

        if (params.min_filter == GL_NEAREST_MIPMAP_NEAREST
            || params.min_filter == GL_NEAREST_MIPMAP_LINEAR
            || params.min_filter == GL_LINEAR_MIPMAP_NEAREST
            || params.min_filter == GL_LINEAR_MIPMAP_LINEAR
        ) {
            generate_mipmaps(params);
        }

        update_params(params);
    }

}