#include <api/image.h>
#include <api/device.h>

#include <io/image_loader.h>

#include <stb_image_resize.h>

namespace gl {

    static int PIXEL_FORMATS[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int INTERNAL_FORMATS[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA };
    static int INTERNAL_FORMATS_SRGB[2] = {GL_SRGB, GL_SRGB_ALPHA };

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

    void ImageBuffer::initHandle() {
        handle = glGetTextureHandleARB(id);

        if (handle == InvalidImageHandle) {
            handle = glGetTextureHandleNV(id);
            glMakeTextureHandleResidentNV(id);
        } else {
            glMakeTextureHandleResidentARB(id);
        }
    }

    void ImageBuffer::initSamplerHandle(u32 sampler) {
        handle = glGetTextureSamplerHandleARB(id, sampler);

        if (handle == InvalidImageHandle) {
            handle = glGetTextureSamplerHandleNV(id, sampler);
            glMakeTextureHandleResidentNV(id);
        } else {
            glMakeTextureHandleResidentARB(id);
        }
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

    void ImageBuffer::updateParams(const ImageParams &params) {
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

        initHandle();
    }

    void ImageBuffer::bindParams(const ImageParams &params) {
        glBindTexture(type, id);
        updateParams(params);
    }

    void ImageBuffer::store(const Image &image) {
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

    void ImageBuffer::storeCubemap(const Image &image) {
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

    void ImageBuffer::load(const Image &image, const ImageParams& params) {
        bind();
        store(image);
        updateParams(params);
    }

    void ImageBuffer::loadCubemap(const Image &image, const ImageParams& params) {
        bind();
        storeCubemap(image);
        updateParams(params);
    }

    void ImageBuffer::loadCubemap(const std::array<Image, 6> &images, const ImageParams& params) {
        bind();
        storeCubemap(images);
        updateParams(params);
    }

    void ImageBuffer::setUnpackAlignment(int alignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    }

    void ImageBuffer::initImageHandle(const AccessMode access, int internalFormat) {
        handle = glGetImageHandleARB(id, 0, GL_FALSE, 0, internalFormat);

        if (handle == InvalidImageHandle) {
            handle = glGetImageHandleNV(id, 0, GL_FALSE, 0, internalFormat);
            glMakeImageHandleResidentNV(handle, access);
        } else {
            glMakeImageHandleResidentARB(handle, access);
        }
    }

    void ImageBuffer::bindImage(int slot, const AccessMode access, int internalFormat) {
        ImageBuffer::bindActivate(type, id, slot);
        glBindImageTexture(slot, id, 0, GL_FALSE, 0, access, internalFormat);
    }

    void ImageBuffer::loadHDR(const char* filepath, const bool flipUV) {
        Image hdrImage = ImageReader::read(filepath, flipUV, PixelType::FLOAT);
        hdrImage.internalFormat = GL_RGB16F;
        hdrImage.pixelFormat = GL_RGB;

        ImageParams params;
        params.minFilter = GL_LINEAR;
        params.magFilter = GL_LINEAR;
        params.s = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;

        init();
        load(hdrImage, params);

        hdrImage.free();
    }

    void ImageBuffer::makeResident() {
        glMakeTextureHandleResidentARB(handle);
        glMakeTextureHandleResidentNV(handle);
    }

    void ImageBuffer::makeNonResident() {
        glMakeTextureHandleNonResidentARB(handle);
        glMakeTextureHandleNonResidentNV(handle);
    }

    void ImageBuffer::makeImageResident(const AccessMode access) {
        glMakeImageHandleResidentARB(handle, access);
        glMakeImageHandleResidentNV(handle, access);
    }

    void ImageBuffer::makeImageNonResident() {
        glMakeImageHandleNonResidentARB(handle);
        glMakeImageHandleNonResidentNV(handle);
    }

}