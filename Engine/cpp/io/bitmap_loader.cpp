#include <io/bitmap_loader.h>

namespace gl {

    void BitmapWriter::write(const char* filepath, const Bitmap& bitmap) {
        u8* bitmapBuffer = (u8*) bitmap.pixels;
        int bufferSize = bitmap.paddedSize;
        int width = bitmap.width;
        int height = bitmap.height;

        BitmapFileHeader bmfh;
        bmfh.type = 0x4d42; // 0x4d42 = 'BM'
        bmfh.size = 14 + sizeof(BitmapHeaderInfo) + bufferSize;
        bmfh.reserved1 = 0; // not used
        bmfh.reserved2 = 0; // not used
        bmfh.offset = 54;

        BitmapHeaderInfo bmih;
        bmih.headerSize = sizeof(BitmapHeaderInfo);
        bmih.width = width;
        bmih.height = height;
        bmih.planes = 1;
        bmih.bitsPerPixel = 24;
        bmih.compression = 0;
        bmih.imageSize = 0;
        bmih.xPixelsPerMeter = 0;
        bmih.yPixelsPerMeter = 0;
        bmih.colorsUsed = 0;
        bmih.importantColorsUsed = 0;

        FILE* file;
        fopen_s(&file, filepath, "wb");
        if (!file) {
            error("Failed to write to {0}", filepath);
            exception("Failed to write bitmap into file");
        }

        // write file header
        fwrite(&bmfh.type, sizeof(short), 1, file);
        fwrite(&bmfh.size, sizeof(int), 1, file);
        fwrite(&bmfh.reserved1, sizeof(short), 1, file);
        fwrite(&bmfh.reserved2, sizeof(short), 1, file);
        fwrite(&bmfh.offset, sizeof(int), 1, file);

        // write info header
        fwrite(&bmih, sizeof(BitmapHeaderInfo), 1, file);

        // write data
        fwrite(bitmapBuffer, sizeof(u8), bufferSize, file);

        fclose(file);
    }

    void Bitmap::initBmp() {
        int rowSize = ((24 * width + 31) / 32) * 4;
        int padding = 0;
        while ((width * 3 + padding) % 4 != 0) padding++;

        u8* buffer = (u8*) pixels;
        paddedSize = rowSize * height;
        u8* bitmapBuffer = (u8*) malloc(paddedSize);

        for (int y = 0; y < height; ++y ) {
            for (int x = 0; x < width; ++x) {
                // position in buffer
                int pos = y * width * 4 + x * 4;
                // position in padded bitmap buffer
                int newPos = (height - y - 1) * (width * 3 + padding) + x * 3;
                bitmapBuffer[newPos + 0] = buffer[pos + 2]; // swap r and b
                bitmapBuffer[newPos + 1] = buffer[pos + 1]; // g stays
                bitmapBuffer[newPos + 2] = buffer[pos + 0]; // swap b and r
            }
        }

        free();

        pixels = bitmapBuffer;
    }

}