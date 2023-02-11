#include <texture.h>
#include <file_utils.h>

namespace gl {

    u32 texture2d_init(
            const char* filepath,
            bool flip_uv,
            int param_s, int param_t,
            const glm::vec4& border_color,
            int min_filter,
            int mag_filter)
    {
        io::image_data texture_data = io::image_read(filepath, flip_uv);
        if (!texture_data.data)
            return 0;

        u32 tbo;
        glGenTextures(1, &tbo);
        glBindTexture(GL_TEXTURE_2D, tbo);

        int width = texture_data.width;
        int height = texture_data.height;
        int channels = texture_data.channels;
        void* data = texture_data.data;

        int data_format = GL_RGB;
        int internal_format = GL_RGB;

        switch (channels) {
            case 1:
                data_format = GL_RED;
                internal_format = GL_RED;
            case 2:
                data_format = GL_RG;
                internal_format = GL_RG;
            case 4:
                data_format = GL_RGBA;
                internal_format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(border_color));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

        free(data);

        return tbo;
    }

    void texture_bind(u32 tbo, u32 type, u32 slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(type, tbo);
    }

}