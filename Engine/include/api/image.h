#pragma once

#include <glad/glad.h>

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

    struct GABRIEL_API Image {
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

    struct GABRIEL_API ImageFace final {
        const char* filepath;
        int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        bool flipUV = false;
        bool srgb = false;
        PixelType pixelType = PixelType::U8;
    };

    struct GABRIEL_API ImageParams final {
        int s = GL_REPEAT;
        int t = GL_REPEAT;
        int r = GL_REPEAT;
        glm::vec4 borderColor = {0, 0, 0, 1 };
        int minFilter = GL_LINEAR;
        int magFilter = GL_LINEAR;
        float lodBias = -0.4f;
        float baseLevel = 0;
    };

    struct GABRIEL_API ImageSampler final {
        const char* name;
        int slot;
    };

#define InvalidImageBuffer 0
#define InvalidImageHandle 0

    struct GABRIEL_API ImageBuffer final {
        u32 id = InvalidImageBuffer;
        u32 type = GL_TEXTURE_2D;
        u64 handle = InvalidImageHandle;

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
        void updateParams(const ImageParams& params);
        void generateMipmaps(const ImageParams& params);

        void store(const Image& image);
        void storeCubemap(const Image& image);
        void storeCubemap(const std::array<Image, 6> &images);

        void load(const Image &image, const ImageParams& params = {});
        void loadCubemap(const Image &image, const ImageParams& params = {});
        void loadCubemap(const std::array<Image, 6> &images, const ImageParams& params = {});

        void bindImage(int slot, const AccessMode access, int internalFormat);

        static void setUnpackAlignment(int alignment);

        void loadHDR(const char* filepath, const bool flipUV);

        void makeResident();
        void makeNonResident();

        void makeImageResident(const AccessMode access);
        void makeImageNonResident();

    private:
        void initHandle();
        void initSamplerHandle(u32 sampler);
        void initImageHandle(const AccessMode access, int internalFormat);

    };

}