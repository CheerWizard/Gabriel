#include <features/displacement.h>

#include <io/image_loader.h>

namespace gl {

    void DisplacementImageMixer::addImage(const DisplacementRange& range, const char* filepath, bool flipUV, PixelType pixelType) {
        displacementImages.emplace_back(ImageReader::read(filepath, flipUV, pixelType), range);
    }

    void DisplacementImageMixer::mix(int width, int height) {
        if (!displacementMap) {
            error("Displacement map is NULL!");
            return;
        }

        if (displacementImages.empty()) {
            error("Displacement images are empty!");
            return;
        }

        mixedImage.width = width;
        mixedImage.height = height;
        mixedImage.channels = 4;
        mixedImage.pixelType = PixelType::U8;
        mixedImage.srgb = false;
        mixedImage.free();
        mixedImage.init();
        mixedImage.setFormat();

        u8* pixels = (u8*) mixedImage.pixels;
        size_t imageSize = displacementImages.size();
        float displacementMapRatio = (float) displacementMap->rows / (float) height;

//        resizeImages(width, height);

        for (int y = 0 ; y < height ; y++) {
            for (int x = 0 ; x < width ; x++) {
                float interpolatedHeight = displacementMap->getInterpolatedHeight(
                        (float) x * displacementMapRatio,
                        (float) y * displacementMapRatio
                );
                float red = 0;
                float green = 0;
                float blue = 0;

                for (int i = 0 ; i < imageSize ; i++) {
                    auto& image = displacementImages[i].image;
                    glm::vec4 color = image.getColor(x, y);
                    float blendFactor = regionPercent(i, interpolatedHeight);

                    red += blendFactor * color.r;
                    green += blendFactor * color.g;
                    blue += blendFactor * color.b;
                }

                if (red >= 255.0f || green >= 255.0f || blue >= 255.0f) {
                    error("RGB color out of bounds!");
                    return;
                }

                pixels[0] = (u8) red;
                pixels[1] = (u8) green;
                pixels[2] = (u8) blue;
                pixels[3] = 255;
                pixels += 4;
            }
        }
    }

    void DisplacementImageMixer::resizeImages(int width, int height) {
        for (auto& displacement_image : displacementImages) {
            displacement_image.image.resize(width, height);
        }
    }

    float DisplacementImageMixer::regionPercent(int tile, float height) {
        float percent = 0.0f;
        auto& range = displacementImages[tile].range;

        if (height < range.min || height > range.max) {
            percent = 0.0f;
        }
        else if (height < range.median) {
            float nom = height - range.min;
            float denom = range.median - range.min;
            percent = nom / denom;
        }
        else if (height >= range.median) {
            float nom = range.max - height;
            float denom = range.max - range.median;
            percent = nom / denom;
        }
        else {
            error("Not able to get height {0} percent of tile {1}", height, tile);
        }

        if (percent < 0.0f || percent > 1.0f) {
            error("Invalid percent {0}", percent);
        }

        return percent;
    }

    HeightMap::HeightMap(const Image &image) : DisplacementMap(image.height, image.width) {
        int channels = image.channels;
        u8* pixels = (u8*) image.pixels;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                // retrieve texel for (i, j) uv
                u8* texel = pixels + (j + columns * i) * channels;
                // raw height at coordinate
                float h = (float) texel[0] / 255.0f;

                if (min > h) {
                    min = h;
                }
                else if (max < h) {
                    max = h;
                }

                get(i, j) = h;
            }
        }
    }

    FaultFormation::FaultFormation(int columns, int rows, int iterations, float minHeight, float maxHeight, float filter)
    : DisplacementMap(rows, columns) {
        float deltaHeight = maxHeight - minHeight;
        min = minHeight;
        max = maxHeight;

        for (int i = 0 ; i < iterations ; i++) {
            float iterationRatio = ((float) i / (float) iterations);
            float h = maxHeight - iterationRatio * deltaHeight;

            int rx1 = rand() % columns;
            int rz1 = rand() % rows;
            int rx2 = rand() % columns;
            int rz2 = rand() % rows;

            int r = 0;
            while (rx1 == rx2 && rz1 == rz2) {
                rx2 = rand() % columns;
                rz2 = rand() % rows;

                if (r++ == 1000) {
                    error("Infinite randomize loop. Breaking loop");
                    break;
                }
            }

            int drx = rx2 - rx1;
            int drz = rz2 - rz1;

            for (int z = 0 ; z < rows ; z++) {
                for (int x = 0 ; x < columns ; x++) {
                    int dx = x - rx1;
                    int dz = z - rz1;
                    int cross = dx * drz - drx * dz;

                    if (cross > 0) {
                        get(z, x) += h;
                    }
                }
            }
        }

        normalize();
        applyFirFilter(filter);
    }

    void FaultFormation::applyFirFilter(float filter) {
        // left to right
        for (int z = 0 ; z < rows ; z++) {
            float& prev = get(z, 0);
            for (int x = 1 ; x < columns ; x++) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
        // right to left
        for (int z = 0 ; z < rows ; z++) {
            float& prev = get(z, columns - 1);
            for (int x = columns - 2 ; x >= 0 ; x--) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
        // bottom to top
        for (int x = 0 ; x < columns ; x++) {
            float& prev = get(0, x);
            for (int z = 1 ; z < rows ; z++) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
        // top to bottom
        for (int x = 0 ; x < columns ; x++) {
            float& prev = get(rows - 1, x);
            for (int z = rows - 2 ; z >= 0 ; z--) {
                get(z, x) = filter * prev + (1 - filter) * get(z, x);
            }
        }
    }

    MidPointFormation::MidPointFormation(int rows, int columns, float roughness, float minHeight, float maxHeight)
    : DisplacementMap(rows, columns) {

        min = minHeight;
        max = maxHeight;

        int rectSize = 1;
        if (rows == 1) {
            rectSize = 2;
        } else {
            while (rectSize < rows) {
                rectSize = rectSize * 2;
            }
        }

        float currentHeight = (float) rectSize / 2.0f;
        float heightReduce = pow(2.0f, -roughness);

        while (rectSize > 0) {
            diamondStep(rectSize, currentHeight);
            squareStep(rectSize, currentHeight);
            rectSize /= 2;
            currentHeight *= heightReduce;
        }

        normalize();
    }

    void MidPointFormation::diamondStep(int rectSize, float height) {
        int halfRectSize = rectSize / 2;
        for (int y = 0 ; y < columns ; y += rectSize) {
            for (int x = 0 ; x < rows ; x += rectSize) {
                int nextX = (x + rectSize) % rows;
                int nextY = (y + rectSize) % columns;

                if (nextX < x) {
                    nextX = rows - 1;
                }

                if (nextY < y) {
                    nextY = columns - 1;
                }

                float topLeft = get(x, y);
                float topRight = get(nextX, y);
                float botLeft = get(x, nextY);
                float botRight = get(nextX, nextY);

                int midX = (x + halfRectSize) % rows;
                int midY = (y + halfRectSize) % columns;

                float r = random<float>(-height, height);
                float midPoint = (topLeft + topRight + botLeft + botRight) / 4.0f;

                get(midX, midY) = midPoint + r;
            }
        }
    }

    void MidPointFormation::squareStep(int rectSize, float height) {
        int halfRectSize = rectSize / 2;

        for (int y = 0 ; y < columns ; y += rectSize) {
            for (int x = 0 ; x < rows ; x += rectSize) {
                int nextX = (x + rectSize) % rows;
                int nextY = (y + rectSize) % columns;

                int midX = (x + halfRectSize) % rows;
                int midY = (y + halfRectSize) % columns;

                int prevMidX = (x - halfRectSize + rows) % rows;
                int prevMidY = (y - halfRectSize + columns) % columns;

                float currentTopLeft = get(x, y);
                float currentTopRight = get(nextX, y);
                float currentBotLeft = get(x, nextY);
                float currentCenter = get(midX, midY);
                float prevXCenter = get(prevMidX, midY);
                float prevYCenter = get(midX, prevMidY);

                float currentMidLeft = (currentTopLeft + currentCenter + currentBotLeft + prevXCenter) / 4.0f + random<float>(-height, height);
                float currentMidTop = (currentTopLeft + currentCenter + currentTopRight + prevYCenter) / 4.0f + random<float>(-height, height);

                get(midX, y) = currentMidTop;
                get(x, midY) = currentMidLeft;
            }
        }
    }

    void DisplacementMap::normalize() {
        float minHeight = map[0];
        float maxHeight = map[0];

        for (int i = 1 ; i < rows * columns ; i++) {

            if (map[i] < minHeight) {
                minHeight = map[i];
            }

            if (map[i] > maxHeight) {
                maxHeight = map[i];
            }

        }

        if (maxHeight <= minHeight)
            return;

        float minMaxDelta = maxHeight - minHeight;
        float minMaxRange = max - min;

        for (int i = 0 ; i < rows * columns; i++) {
            map[i] = ((map[i] - minHeight) / minMaxDelta) * minMaxRange + min;
        }
    }

    float DisplacementMap::getInterpolatedHeight(float x, float y) {
        float height = get((int)x, (int)y);

        if (((int) x + 1 >= rows) || ((int) y + 1 >= columns)) {
            return height;
        }

        float heightNextX = get((int)x + 1, (int)y);
        float ratioX = x - floorf(x);
        float heightInterpolatedX = (heightNextX - height) * ratioX + height;

        float heightNextY = get((int)x, (int)y + 1);
        float ratioY = y - floorf(y);
        float heightInterpolatedY = (heightNextY - height) * ratioY + height;

        return (heightInterpolatedX + heightInterpolatedY) / 2.0f;
    }
}