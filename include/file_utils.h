#pragma once

#include <primitives.h>
#include <fstream>
#include <string>

#include <stb_image.h>

namespace io {

    std::string file_read(const char* filepath);

    struct image_data final {
        int width, height, channels;
        u8* data;
    };

    image_data image_read(const char* filepath, bool flip_uv = false);

}
