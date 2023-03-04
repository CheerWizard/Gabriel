#pragma once

#include <primitives.h>
#include <fstream>
#include <string>

#include <stb_image.h>

namespace io {

    std::string read_file_string(const char* filepath);

    struct ImageData final {
        int width, height, channels;
        u8* data;
    };

    struct HDRImageData final {
        int width, height, channels;
        float* data;
    };

    ImageData read_image(const char* filepath, bool flip_uv = false);
    HDRImageData read_hdr_image(const char* filepath, bool flip_uv = false);
    void free(void* data);

}
