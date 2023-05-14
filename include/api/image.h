#pragma once

#include <core/common.h>

#include <array>
#include <fstream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

namespace gl {

    enum AccessMode : u32 {
        READ_ONLY = GL_READ_ONLY,
        WRITE_ONLY = GL_WRITE_ONLY,
        READ_WRITE = GL_READ_WRITE
    };

    enum PixelType : u32 {
        U8 = GL_UNSIGNED_BYTE,
        U16 = GL_UNSIGNED_SHORT,
        FLOAT = GL_FLOAT,
        UINT248 = GL_UNSIGNED_INT_24_8,
        U32 = GL_UNSIGNED_INT
    };

    struct Image {
        int width, height;
        int channels;
        int internalFormat, pixelFormat;
        PixelType pixelType = PixelType::U8;
        void* pixels = null;
        int samples = 1;
        bool srgb = false;

        void init();
        void free();

        void resize(int w, int h);

        glm::vec4 getColor(int x, int y);

        void setFormat();

        size_t size();
    };

    struct ImageFace final {
        const char* filepath;
        int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        bool flipUV = false;
        bool srgb = false;
        PixelType pixelType = PixelType::U8;
    };

    struct ImageReader final {
        static Image read(const char* filepath, bool flipUV = false, PixelType pixelType = PixelType::U8, bool srgb = false);
        static std::array<Image, 6> read(const std::array<ImageFace, 6> &faces);
    };

    struct Bitmap : Image {
        int paddedSize = 0;

        void initBmp();
    };

    struct BitmapFileHeader final {
        short type;
        int size;
        short reserved1;
        short reserved2;
        int offset;
    };

    struct BitmapHeaderInfo final {
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

    struct ImageWriter final {
        static void write(const char* filepath, const Image& image);
        static bool write(const char* filepath, const Bitmap& bitmap);
    };

    struct ImageParams final {
        int s = GL_REPEAT;
        int t = GL_REPEAT;
        int r = GL_REPEAT;
        glm::vec4 borderColor = {0, 0, 0, 1 };
        int minFilter = GL_LINEAR;
        int magFilter = GL_LINEAR;
        float lodBias = -0.4f;
        float baseLevel = 0;
    };

    struct ImageSampler final {
        const char* name;
        int slot;
    };

#define InvalidImageBuffer 0

    struct ImageBuffer final {
        u32 id = InvalidImageBuffer;
        u32 type = GL_TEXTURE_2D;

        ImageBuffer() = default;

        ImageBuffer(u32 type) : type(type) {}

        void init();
        void free();
        void free(int size);

        void bind() const;
        void bindActivate(int slot) const;
        static void bind(u32 type, u32 id);
        static void activate(int slot);
        static void bindActivate(u32 type, u32 id, int slot);
        static void unbind();

        void bindParams(const ImageParams& params);
        void updateParams(const ImageParams& params) const;
        void generateMipmaps(const ImageParams& params);

        void store(const Image& image) const;
        void storeCubemap(const Image& image) const;
        static void storeCubemap(const std::array<Image, 6> &images);

        void load(const Image &image, const ImageParams& params = {}) const;
        void loadCubemap(const Image &image, const ImageParams& params = {}) const;
        void loadCubemap(const std::array<Image, 6> &images, const ImageParams& params = {}) const;

        void bindImage(int slot, AccessMode access, int internalFormat);

        static void setUnpackAlignment(int alignment);
    };

}