#pragma once

#include <geometry/geometry.h>

namespace gl {

    struct GABRIEL_API DisplacementMap {
        int rows;
        int columns;
        float min = 0.0f;
        float max = 1.0f;
        std::vector<float> map;

        DisplacementMap() = default;

        DisplacementMap(int rows, int columns) : rows(rows), columns(columns) {
            map.resize(rows * columns);
        }

        inline int size() const { return rows * columns; }

        inline float& get(int row, int col) {
            return map[row * rows + col];
        }

        inline float& operator [](int i) { return map[i]; }

        void normalize();

        float getInterpolatedHeight(float x, float y);
    };

    struct GABRIEL_API DisplacementRange final {
        float min = 0;
        float median = 0.5f;
        float max = 1;
    };

    struct GABRIEL_API DisplacementImage final {
        Image image;
        DisplacementRange range;

        DisplacementImage() = default;
        DisplacementImage(const Image& image) : image(image) {}
        DisplacementImage(const Image& image, const DisplacementRange& range) : image(image), range(range) {}
    };

    struct GABRIEL_API DisplacementImageMixer final {
        std::vector<DisplacementImage> displacementImages;
        Image mixedImage;
        DisplacementMap* displacementMap = null;

        void addImage(const DisplacementRange& range, const char* filepath, bool flipUV = false, PixelType pixelType = PixelType::U8);
        void mix(int width, int height);

    private:
        void resizeImages(int width, int height);
        float regionPercent(int tile, float height);
    };

    struct GABRIEL_API HeightMap : DisplacementMap {
        explicit HeightMap(const Image& image);
    };

    struct GABRIEL_API FaultFormation : DisplacementMap {
        FaultFormation(int columns, int rows, int iterations, float minHeight, float maxHeight, float filter);
    private:
        void applyFirFilter(float filter);
    };

    struct GABRIEL_API MidPointFormation : DisplacementMap {
        MidPointFormation(int row, int columns, float roughness, float minHeight, float maxHeight);

    private:
        void diamondStep(int rectSize, float height);
        void squareStep(int rectSize, float height);
    };

    template<typename T>
    struct Displacement {
        DisplacementMap map;
        float scale = 1.0f;

        void free();

        void setOriginVertices(Vertices<T>* vertices);

        void displace(DrawableElements& drawable);

    private:
        Vertices<T>* mVertices = null;
        Vertices<T> mOriginVertices;
    };

    template<typename T>
    void Displacement<T>::setOriginVertices(Vertices<T>* vertices) {
        this->mVertices = vertices;
        mOriginVertices.copyFrom(vertices);
    }

    template<typename T>
    void Displacement<T>::displace(DrawableElements& drawable) {
        // displace mVertices Y positions
        auto& displacedVertices = *mVertices;
        int size = map.size();
        float s = scale;

        for (int i = 0 ; i < size ; i++) {
            auto& displacedV = displacedVertices[i];
            auto& originV = mOriginVertices[i];
            displacedV.pos = originV.pos + map[i] * s * glm::normalize(originV.normal);
        }

        drawable.vao.bind();
        drawable.vbo.update(*mVertices);
    }

    template<typename T>
    void Displacement<T>::free() {
        mOriginVertices.free();
    }

    component(DisplacementTBN), Displacement<VertexTBN> {
        ~DisplacementTBN() {
            free();
        }
    };

}