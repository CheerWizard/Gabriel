#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <api/image.h>
#include <api/device.h>

namespace gl {

    static int PIXEL_FORMATS[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int INTERNAL_FORMATS[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int INTERNAL_FORMATS_SRGB[2] = {GL_SRGB, GL_SRGB_ALPHA };

    Image ImageReader::read(const char *filepath, bool flipUV, PixelType pixelType, bool srgb) {
        Image image;

        stbi_set_flip_vertically_on_load(flipUV);

        switch (pixelType) {
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
            error("Failed to read image {0}", filepath);
            return image;
        }

        image.pixelType = pixelType;
        image.srgb = srgb;
        image.setFormat();

        return image;
    }

    std::array<Image, 6> ImageReader::read(const std::array<ImageFace, 6> &faces) {
        std::array<Image, 6> images;

        for (int i = 0 ; i < 6 ; i++) {
            auto& face = faces[i];
            images[i] = ImageReader::read(face.filepath, face.flipUV, face.pixelType, face.srgb);
        }

        return images;
    }

    void Image::init() {
        pixels = malloc(width * height * channels);
    }

    void Image::free() {
        ::free(pixels);
    }

    void Image::resize(int w, int h) {
        if (width == w || height == h)
            return;

        void* resizedData;

        if (pixelType == PixelType::U8) {
            resizedData = malloc(width * height * sizeof(u8));
            int status = stbir_resize_uint8((u8*) pixels, width, height, 0, (u8*) resizedData, w, h, 0, channels);
            if (!status) {
                error("Failed to resize 8-bit pixels [{0}, {1}]", w, h);
                return;
            }
        }

        else if (pixelType == PixelType::U16) {
            // todo consider how to resize 16-bit pixels
            resizedData = malloc(width * height * sizeof(u16));
//            int status = stbir_resize_uint16_generic((u16*) data, width, height, 0, (u16*) resizedData, w, h, 0, channels);
//            if (!status) {
//                print_err("Image::resize: failed to resize 16-bit pixels [ " << width << " , " << height << "] -> [ , ] << w << ", height" << h);
//                return;
//            }
        }

        else if (pixelType == PixelType::FLOAT) {
            resizedData = malloc(width * height * sizeof(float));
            int status = stbir_resize_float((float*) pixels, width, height, 0, (float*) resizedData, w, h, 0, channels);
            if (!status) {
                error("Failed to resize 32-bit pixels [{0}, {1}]", w, h);
                return;
            }
        }

        else {
            error("Failed to resize unknown image type!");
            return;
        }

        free();

        pixels = resizedData;
        width = w;
        height = h;
    }

    glm::vec4 Image::getColor(int x, int y) {
        u8* p = (u8*) pixels;
        u8* pixel = p + (x + width * y) * channels;
        u8 r = pixel[0];
        u8 g = pixel[1];
        u8 b = pixel[2];
        u8 a = channels >= 4 ? pixel[3] : 0xff;
        return { r, g, b, a };
    }

    void Image::setFormat() {
        pixelFormat = PIXEL_FORMATS[channels - 1];
        if (srgb) {
            internalFormat = INTERNAL_FORMATS_SRGB[channels - 3];
        } else {
            internalFormat = INTERNAL_FORMATS[channels - 1];
        }
    }

    size_t Image::size() {
        return width * height * channels;
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

    void ImageBuffer::bindActivate(int slot) const {
        activate(slot);
        bind(type, id);
    }

    void ImageBuffer::bind(u32 type, u32 id) {
        glBindTexture(type, id);
    }

    void ImageBuffer::activate(int slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
    }

    void ImageBuffer::bindActivate(u32 type, u32 id, int slot) {
        activate(slot);
        bind(type, id);
    }

    void ImageBuffer::unbind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ImageBuffer::generateMipmaps(const ImageParams &params) {
        u32 textureType = type;
        glBindTexture(textureType, id);
        glGenerateMipmap(textureType);
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTexParameterf(textureType, GL_TEXTURE_LOD_BIAS, params.lodBias);
        glTexParameterf(textureType, GL_TEXTURE_BASE_LEVEL, params.baseLevel);
        glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, Device::get().MAX_ANISOTROPY_SAMPLES);
    }

    void ImageBuffer::updateParams(const ImageParams &params) const {
        u32 textureType = type;
        int minFilter = params.minFilter;

        if (minFilter == GL_NEAREST_MIPMAP_NEAREST
            || minFilter == GL_NEAREST_MIPMAP_LINEAR
            || minFilter == GL_LINEAR_MIPMAP_NEAREST
            || minFilter == GL_LINEAR_MIPMAP_LINEAR
        ) {
            glGenerateMipmap(textureType);
            glTexParameterf(textureType, GL_TEXTURE_LOD_BIAS, params.lodBias);
            glTexParameterf(textureType, GL_TEXTURE_BASE_LEVEL, params.baseLevel);
            glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, Device::get().MAX_ANISOTROPY_SAMPLES);
        }

        glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(params.borderColor));
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, params.s);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, params.t);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_R, params.r);
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, params.magFilter);
    }

    void ImageBuffer::bindParams(const ImageParams &params) {
        glBindTexture(type, id);
        updateParams(params);
    }

    void ImageBuffer::store(const Image &image) const {
        int internalFormat = image.internalFormat;
        if (image.srgb) {
            internalFormat = INTERNAL_FORMATS_SRGB[image.channels - 3];
        }

        if (type == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(
                    type,
                    image.samples,
                    internalFormat,
                    image.width, image.height,
                    GL_TRUE
            );
        } else {
            glTexImage2D(
                    type, 0,
                    internalFormat,
                    image.width, image.height, 0,
                    image.pixelFormat,
                    image.pixelType,
                    image.pixels
            );
        }
    }

    void ImageBuffer::storeCubemap(const Image &image) const {
        for (int i = 0 ; i < 6 ; i++) {
            int internalFormat = image.internalFormat;
            if (image.srgb) {
                internalFormat = INTERNAL_FORMATS_SRGB[image.channels - 3];
            }

            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    internalFormat,
                    image.width, image.height, 0,
                    image.pixelFormat,
                    image.pixelType,
                    image.pixels
            );
        }
    }

    void ImageBuffer::storeCubemap(const std::array<Image, 6> &images) {
        for (int i = 0 ; i < 6 ; i++) {
            auto& image = images[i];

            int internalFormat = image.internalFormat;
            if (image.srgb) {
                internalFormat = INTERNAL_FORMATS_SRGB[image.channels - 3];
            }

            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    internalFormat,
                    image.width, image.height,
                    0, image.pixelFormat,
                    image.pixelType,
                    image.pixels
            );
        }
    }

    void ImageBuffer::load(const Image &image, const ImageParams& params) const {
        bind();
        store(image);
        updateParams(params);
    }

    void ImageBuffer::loadCubemap(const Image &image, const ImageParams& params) const {
        bind();
        storeCubemap(image);
        updateParams(params);
    }

    void ImageBuffer::loadCubemap(const std::array<Image, 6> &images, const ImageParams& params) const {
        bind();
        storeCubemap(images);
        updateParams(params);
    }

    void ImageBuffer::setUnpackAlignment(int alignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    }

    void ImageBuffer::bindImage(int slot, AccessMode access, int internalFormat) {
        ImageBuffer::bindActivate(type, id, slot);
        glBindImageTexture(slot, id, 0, GL_FALSE, 0, access, internalFormat);
    }

    void ImageWriter::write(const char* filepath, const Image &image) {
        stbi_write_png(filepath, image.width, image.height, image.channels, image.pixels, image.width * image.channels);
    }

    bool ImageWriter::write(const char* filepath, const Bitmap& bitmap) {
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
        fwrite(bitmapBuffer, sizeof(u8), bufferSize, file);

        fclose(file);

        return true;
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