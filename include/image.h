#pragma once

#include <primitives.h>

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
        int internal_format, pixel_format;
        PixelType pixel_type = PixelType::U8;
        void* pixels = null;
        int samples = 1;
        bool srgb = false;

        void init();

        void free();

        void resize(int w, int h);

        glm::vec4 get_color(int x, int y);

        void set_format();
    };

    struct ImageFace final {
        const char* filepath;
        int face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        bool flip_uv = false;
        bool srgb = false;
        PixelType pixel_type = PixelType::U8;
    };

    struct ImageReader final {
        static Image read(const char* filepath, bool flip_uv = false, PixelType pixel_type = PixelType::U8, bool srgb = false);
        static std::array<Image, 6> read(const std::array<ImageFace, 6> &faces);
    };

    struct Bitmap : Image {
        int padded_size = 0;

        void init_bmp();
    };

    struct BitmapFileHeader final {
        short type;
        int size;
        short reserved1;
        short reserved2;
        int offset;
    };

    struct BitmapHeaderInfo final {
        int header_size;
        int width;
        int height;
        short planes;
        short bits_per_pixel;
        int compression;
        int image_size;
        int x_pixels_per_meter;
        int y_pixels_per_meter;
        int colors_used;
        int important_colors_used;
    };

    struct ImageWriter final {
        static void write(const char* filepath, const Image& image);
        static bool write(const char* filepath, const Bitmap& bitmap);
    };

    struct ImageParams final {
        int s = GL_REPEAT;
        int t = GL_REPEAT;
        int r = GL_REPEAT;
        glm::vec4 border_color = { 0, 0, 0, 1 };
        int min_filter = GL_LINEAR;
        int mag_filter = GL_LINEAR;
        float lod_bias = -0.4f;
        float base_level = 0;
    };

    struct ImageSampler final {
        const char* name;
        int slot;
    };

#define invalid_image_buffer 0

    struct ImageBuffer final {
        u32 id = invalid_image_buffer;
        u32 type = GL_TEXTURE_2D;

        ImageBuffer() = default;

        ImageBuffer(u32 type) : type(type) {}

        void init();
        void free();
        void free(int size);

        void bind() const;
        static void bind(u32 type, u32 id);
        static void activate(int slot);
        static void bind_activate(u32 type, u32 id, int slot);
        static void unbind();

        void bind_params(const ImageParams& params);
        void update_params(const ImageParams& params) const;
        void generate_mipmaps(const ImageParams& params);

        void store(const Image& image) const;
        void store_cubemap(const Image& image) const;
        static void store_cubemap(const std::array<Image, 6> &images);

        void load(const Image &image, const ImageParams& params = {}) const;
        void load_cubemap(const Image &image, const ImageParams& params = {}) const;
        void load_cubemap(const std::array<Image, 6> &images, const ImageParams& params = {}) const;

        void bind_image(int slot, AccessMode access, int internal_format);

        static void set_byte_alignment(int alignment);
    };

}