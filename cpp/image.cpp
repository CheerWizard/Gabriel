#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <image.h>
#include <device.h>

namespace gl {

    static int pixel_formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int internal_formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int internal_formats_srgb[2] = { GL_SRGB, GL_SRGB_ALPHA };

    Image ImageReader::read(const char *filepath, bool flip_uv, PixelType pixel_type, bool srgb) {
        Image image;

        stbi_set_flip_vertically_on_load(flip_uv);

        switch (pixel_type) {
            case PixelType::U16:
                image.pixels = stbi_load_16(filepath, &image.width, &image.height, &image.channels, 0);
                break;
            case PixelType::FLOAT:
                image.pixels = stbi_loadf(filepath, &image.width, &image.height, &image.channels, 0);
                break;
            default:
                image.pixels = stbi_load(filepath, &image.width, &image.height, &image.channels, 0);
                break;
        }

        if (!image.pixels) {
            print_err("ImageReader::read(): failed to read image " << filepath);
            return image;
        }

        image.pixel_type = pixel_type;
        image.srgb = srgb;
        image.set_format();

        return image;
    }

    std::array<Image, 6> ImageReader::read(const std::array<ImageFace, 6> &faces) {
        std::array<Image, 6> images;

        for (int i = 0 ; i < 6 ; i++) {
            auto& face = faces[i];
            images[i] = ImageReader::read(face.filepath, face.flip_uv, face.pixel_type, face.srgb);
        }

        return images;
    }

    void Image::init() {
        pixels = malloc(width * height * channels);
    }

    void Image::free() {
        stbi_image_free(pixels);
    }

    void Image::resize(int w, int h) {
        if (width == w || height == h)
            return;

        void* resized_data;

        if (pixel_type == PixelType::U8) {
            resized_data = malloc(width * height * sizeof(u8));
            int status = stbir_resize_uint8((u8*) pixels, width, height, 0, (u8*) resized_data, w, h, 0, channels);
            if (!status) {
                print_err("Image::resize: failed to resize 8-bit pixels [" << w << ", " << h << "]");
                return;
            }
        }

        else if (pixel_type == PixelType::U16) {
            // todo consider how to resize 16-bit pixels
            resized_data = malloc(width * height * sizeof(u16));
//            int status = stbir_resize_uint16_generic((u16*) data, width, height, 0, (u16*) resized_data, w, h, 0, channels);
//            if (!status) {
//                print_err("Image::resize: failed to resize 16-bit pixels [ " << width << " , " << height << "] -> [ , ] << w << ", height" << h);
//                return;
//            }
        }

        else if (pixel_type == PixelType::FLOAT) {
            resized_data = malloc(width * height * sizeof(float));
            int status = stbir_resize_float((float*) pixels, width, height, 0, (float*) resized_data, w, h, 0, channels);
            if (!status) {
                print_err("Image::resize: failed to resize 32-bit pixels [" << w << ", " << h << "]");
                return;
            }
        }

        else {
            print_err("Image::resize: failed to resize unknown image type!");
            return;
        }

        free();

        pixels = resized_data;
        width = w;
        height = h;
    }

    glm::vec4 Image::get_color(int x, int y) {
        u8* p = (u8*) pixels;
        u8* pixel = p + (x + width * y) * channels;
        u8 r = pixel[0];
        u8 g = pixel[1];
        u8 b = pixel[2];
        u8 a = channels >= 4 ? pixel[3] : 0xff;
        return { r, g, b, a };
    }

    void Image::set_format() {
        pixel_format = pixel_formats[channels - 1];
        if (srgb) {
            internal_format = internal_formats_srgb[channels - 3];
        } else {
            internal_format = internal_formats[channels - 1];
        }
    }

    void ImageBuffer::init() {
        glGenTextures(1, &id);
    }

    void ImageBuffer::free() {
        glDeleteTextures(1, &id);
    }

    void ImageBuffer::free(int size) {
        glDeleteTextures(size, &id);
    }

    void ImageBuffer::bind() const {
        glBindTexture(type, id);
    }

    void ImageBuffer::bind(u32 type, u32 id) {
        glBindTexture(type, id);
    }

    void ImageBuffer::activate(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
    }

    void ImageBuffer::bind_activate(u32 type, u32 id, int slot) {
        activate(slot);
        bind(type, id);
    }

    void ImageBuffer::unbind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ImageBuffer::generate_mipmaps(const ImageParams &params) {
        u32 texture_type = type;
        glBindTexture(texture_type, id);
        glGenerateMipmap(texture_type);
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameterf(texture_type, GL_TEXTURE_LOD_BIAS, params.lod_bias);
        glTexParameterf(texture_type, GL_TEXTURE_BASE_LEVEL, params.base_level);
        glTexParameterf(texture_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, Device::MAX_ANISOTROPY_SAMPLES);
    }

    void ImageBuffer::update_params(const ImageParams &params) const {
        u32 texture_type = type;
        int min_filter = params.min_filter;

        if (min_filter == GL_NEAREST_MIPMAP_NEAREST
            || min_filter == GL_NEAREST_MIPMAP_LINEAR
            || min_filter == GL_LINEAR_MIPMAP_NEAREST
            || min_filter == GL_LINEAR_MIPMAP_LINEAR
        ) {
            glGenerateMipmap(texture_type);
            glTexParameterf(texture_type, GL_TEXTURE_LOD_BIAS, params.lod_bias);
            glTexParameterf(texture_type, GL_TEXTURE_BASE_LEVEL, params.base_level);
            glTexParameterf(texture_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, Device::MAX_ANISOTROPY_SAMPLES);
        }

        glTexParameterfv(texture_type, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.border_color));
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, params.r);
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    }

    void ImageBuffer::bind_params(const ImageParams &params) {
        glBindTexture(type, id);
        update_params(params);
    }

    void ImageBuffer::store(const Image &image) const {
        int internal_format = image.internal_format;
        if (image.srgb) {
            internal_format = internal_formats_srgb[image.channels - 3];
        }

        if (type == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(
                    type,
                    image.samples,
                    internal_format,
                    image.width, image.height,
                    GL_TRUE
            );
        } else {
            glTexImage2D(
                    type, 0,
                    internal_format,
                    image.width, image.height, 0,
                    image.pixel_format,
                    image.pixel_type,
                    image.pixels
            );
        }
    }

    void ImageBuffer::store_cubemap(const Image &image) const {
        for (int i = 0 ; i < 6 ; i++) {

            int internal_format = image.internal_format;
            if (image.srgb) {
                internal_format = internal_formats_srgb[image.channels - 3];
            }

            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    internal_format,
                    image.width, image.height, 0,
                    image.pixel_format,
                    image.pixel_type,
                    image.pixels
            );
        }
    }

    void ImageBuffer::store_cubemap(const std::array<Image, 6> &images) {
        for (int i = 0 ; i < 6 ; i++) {
            auto& image = images[i];

            int internal_format = image.internal_format;
            if (image.srgb) {
                internal_format = internal_formats_srgb[image.channels - 3];
            }

            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    internal_format,
                    image.width, image.height,
                    0, image.pixel_format,
                    image.pixel_type,
                    image.pixels
            );
        }
    }

    void ImageBuffer::load(const Image &image, const ImageParams& params) const {
        bind();
        store(image);
        update_params(params);
    }

    void ImageBuffer::load_cubemap(const Image &image, const ImageParams& params) const {
        bind();
        store_cubemap(image);
        update_params(params);
    }

    void ImageBuffer::load_cubemap(const std::array<Image, 6> &images, const ImageParams& params) const {
        bind();
        store_cubemap(images);
        update_params(params);
    }

    void ImageBuffer::set_byte_alignment(int alignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    }

    void ImageBuffer::bind_image(int slot, AccessMode access, int internal_format) {
        ImageBuffer::bind_activate(type, id, slot);
        glBindImageTexture(slot, id, 0, GL_FALSE, 0, access, internal_format);
    }

    void ImageWriter::write(const char* filepath, const Image &image) {
        stbi_write_png(filepath, image.width, image.height, image.channels, image.pixels, image.width * image.channels);
    }

    bool ImageWriter::write(const char* filepath, const Bitmap& bitmap) {
        u8* bitmap_buffer = (u8*) bitmap.pixels;
        int buffer_size = bitmap.padded_size;
        int width = bitmap.width;
        int height = bitmap.height;

        BitmapFileHeader bmfh;
        bmfh.type = 0x4d42; // 0x4d42 = 'BM'
        bmfh.size = 14 + sizeof(BitmapHeaderInfo) + buffer_size;
        bmfh.reserved1 = 0; // not used
        bmfh.reserved2 = 0; // not used
        bmfh.offset = 54;

        BitmapHeaderInfo bmih;
        bmih.header_size = sizeof(BitmapHeaderInfo);
        bmih.width = width;
        bmih.height = height;
        bmih.planes = 1;
        bmih.bits_per_pixel = 24;
        bmih.compression = 0;
        bmih.image_size = 0;
        bmih.x_pixels_per_meter = 0;
        bmih.y_pixels_per_meter = 0;
        bmih.colors_used = 0;
        bmih.important_colors_used = 0;

        FILE* file;
        fopen_s(&file, filepath, "wb");
        if (!file) {
            print_err("Failed to write to " << filepath);
            return false;
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
        fwrite(bitmap_buffer, sizeof(u8), buffer_size, file);

        fclose(file);

        return true;
    }

    void Bitmap::init_bmp() {
        int rowSize = ((24 * width + 31) / 32) * 4;
        int padding = 0;
        while ((width * 3 + padding) % 4 != 0) padding++;

        u8* buffer = (u8*) pixels;
        padded_size = rowSize * height;
        u8* bitmap_buffer = (u8*) malloc(padded_size);

        for (int y = 0; y < height; ++y ) {
            for (int x = 0; x < width; ++x) {
                // position in buffer
                int pos = y * width * 4 + x * 4;
                // position in padded bitmap buffer
                int newPos = (height - y - 1) * (width * 3 + padding) + x * 3;
                bitmap_buffer[newPos + 0] = buffer[pos + 2]; // swap r and b
                bitmap_buffer[newPos + 1] = buffer[pos + 1]; // g stays
                bitmap_buffer[newPos + 2] = buffer[pos + 0]; // swap b and r
            }
        }

        free();

        pixels = bitmap_buffer;
    }
}