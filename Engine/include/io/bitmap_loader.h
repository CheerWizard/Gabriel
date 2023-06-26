#pragma once

#include <api/image.h>

namespace gl {

    struct GABRIEL_API Bitmap : Image {
        int paddedSize = 0;

        void initBmp();
    };

    struct GABRIEL_API BitmapFileHeader final {
        short type;
        int size;
        short reserved1;
        short reserved2;
        int offset;
    };

    struct GABRIEL_API BitmapHeaderInfo final {
        int headerSize;
        int width;
        int height;
        short planes;
        short bitsPerPixel;
        int compression;
        int imageSize;
        int xPixelsPerMeter;
        int yPixelsPerMeter;
        int colorsUsed;
        int importantColorsUsed;
    };

    struct GABRIEL_API BitmapWriter final {
        static void write(const char* filepath, const Bitmap& bitmap);
    };

}