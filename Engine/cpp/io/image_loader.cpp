#include <io/image_loader.h>

#include <stb_image.h>
#include <stb_image_write.h>

namespace gl {

    Image ImageReader::read(const char* filepath, const bool flipUV, const PixelType pixelType, const bool srgb) {
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

    void ImageWriter::write(const char* filepath, const Image &image) {
        stbi_write_png(filepath, image.width, image.height, image.channels, image.pixels, image.width * image.channels);
    }

}