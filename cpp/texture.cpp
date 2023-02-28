#include <texture.h>

namespace gl {

    static int data_formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int internal_formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int internal_formats_srgb[2] = { GL_SRGB, GL_SRGB_ALPHA };

    void texture2d_init(
            texture& texture,
            const char* filepath,
            bool flip_uv,
            const texture_2d_params& params
    ) {
        io::image_data image = io::image_read(filepath, flip_uv);
        if (!image.data)
            return;

        gl::texture_data texture_data;
        texture_data.width = image.width;
        texture_data.height = image.height;
        texture_data.data = image.data;
        texture_data.data_format = data_formats[image.channels - 1];
        if (params.srgb) {
            texture_data.internal_format = internal_formats_srgb[image.channels - 3];
        } else {
            texture_data.internal_format = internal_formats[image.channels - 1];
        }

        texture2d_init(texture, texture_data, params);

        free(image.data);
    }

    void texture2d_init(
            texture& texture,
            const texture_data& texture_data,
            const texture_2d_params& params
    ) {
        u32 texture_type = GL_TEXTURE_2D;
        texture.type = texture_type;
        glGenTextures(1, &texture.id);
        glBindTexture(texture_type, texture.id);

        int width = texture_data.width;
        int height = texture_data.height;
        int data_format = texture_data.data_format;
        int internal_format = texture_data.internal_format;
        u32 primitive_type = texture_data.primitive_type;
        void* data = texture_data.data;

        glTexImage2D(texture_type, 0, internal_format, width, height, 0, data_format, primitive_type, data);
        if (params.generate_mipmap) {
            glGenerateMipmap(texture_type);
        }

        glTexParameterfv(texture_type, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.border_color));
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    }

    void texture_free(u32 tbo) {
        glDeleteTextures(1, &tbo);
    }

    void textures_free(u32* tbo, int size) {
        glDeleteTextures(size, tbo);
    }

    void texture_update(u32 shader, texture& texture) {
        shader_set_uniform(shader, texture.sampler);
        glActiveTexture(GL_TEXTURE0 + texture.sampler.value);
        glBindTexture(texture.type, texture.id);
    }

    void texture_update(const texture& texture, const texture_2d_params& params) {
        glBindTexture(texture.type, texture.id);
        glTexParameterfv(texture.type, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.border_color));
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    }

    void texture_update(const texture& texture, const texture_3d_params& params) {
        glBindTexture(texture.type, texture.id);
        glTexParameterfv(texture.type, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.border_color));
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_R, params.r);
        glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    }

    void texture_cube_init(
            texture& texture,
            const std::array<texture_face, 6>& faces,
            const texture_3d_params& params
    ) {
        texture.type = GL_TEXTURE_CUBE_MAP;
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);

        for (const auto& face : faces) {
            io::image_data texture_data = io::image_read(face.filepath, face.flip_uv);
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

            free(data);
        }

        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.border_color));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, params.r);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    }

}