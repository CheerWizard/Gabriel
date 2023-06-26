#pragma once

#include <api/image.h>

namespace gl {

    struct GABRIEL_API ImageReader final {
        static Image read(const char* filepath, const bool flipUV = false, const PixelType pixelType = PixelType::U8, const bool srgb = false);
        static std::array<Image, 6> read(const std::array<ImageFace, 6> &faces);
    };

    struct GABRIEL_API ImageWriter final {
        static void write(const char* filepath, const Image& image);
    };

}