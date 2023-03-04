#define STB_IMAGE_IMPLEMENTATION
#include <file_utils.h>

namespace io {

    std::string read_file_string(const char* filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            print_err("read_file_string() : failed to open file");
            print_err(filepath);
            return "";
        }

        std::string buffer;
        std::string line;
        while (std::getline(file, line)) {
            buffer.append(line);
            buffer.append("\n");
            if (file.eof())
                break;
        }

        file.close();

        return buffer;
    }

    ImageData read_image(const char* filepath, bool flip_uv) {
        ImageData result;

        stbi_set_flip_vertically_on_load(flip_uv);

        result.data = stbi_load(filepath, &result.width, &result.height, &result.channels, 0);
        if (!result.data) {
            print_err("image_read(): failed to read image " << filepath);
            return result;
        }

        return result;
    }

    HDRImageData read_hdr_image(const char* filepath, bool flip_uv) {
        HDRImageData result;

        stbi_set_flip_vertically_on_load(flip_uv);

        result.data = stbi_loadf(filepath, &result.width, &result.height, &result.channels, 0);
        if (!result.data) {
            print_err("image_read(): failed to read image " << filepath);
            return result;
        }

        return result;
    }

    void free(void* data) {
        stbi_image_free(data);
    }

}