#pragma once

#include <geometry/geometry.h>

namespace gl {

    template<typename T>
    struct SphereGeometry : Geometry<T> {
        int xSegments = 64;
        int ySegments = 64;

        SphereGeometry() = default;

        SphereGeometry(int xSegments, int ySegments) : xSegments(xSegments), ySegments(ySegments) {
            generate();
        }

        void generate();
        void init(DrawableElements& drawable);
    };

    struct SphereUV : SphereGeometry<VertexUV> {
        SphereUV() = default;
        SphereUV(int xSegments, int ySegments) : SphereGeometry<VertexUV>(xSegments, ySegments) {}
    };

    struct SphereTBN : SphereGeometry<VertexTBN> {
        SphereTBN() = default;
        SphereTBN(int xSegments, int ySegments) : SphereGeometry<VertexTBN>(xSegments, ySegments) {
            generateTBN();
        }

        void generateTBN();
    };

    template<typename T>
    void SphereGeometry<T>::generate() {
        this->vertices.init((xSegments + 1) * (ySegments + 1));

        int i = 0;
        for (u32 x = 0; x <= xSegments; x++) {
            for (u32 y = 0; y <= ySegments; y++) {
                T& V = this->vertices[i];

                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;

                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                V.pos = { xPos, yPos, zPos };
                V.uv = { xSegment, ySegment };

                i++;
            }
        }

        bool oddRow = false;
        Indices newIndices;
        std::vector<u32> indexList;

        for (u32 y = 0; y < ySegments; y++) {
            if (!oddRow) {
                for (u32 x = 0; x <= xSegments; x++) {
                    indexList.push_back(y * (xSegments + 1) + x);
                    indexList.push_back((y + 1) * (xSegments + 1) + x);
                }
            }
            else {
                for (int x = xSegments; x >= 0; x--) {
                    indexList.push_back((y + 1) * (xSegments + 1) + x);
                    indexList.push_back(y * (xSegments + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        newIndices.indices = indexList.data();
        newIndices.count = (int) indexList.size();
        this->indices.copyFrom(&newIndices);
    }

    template<typename T>
    void SphereGeometry<T>::init(DrawableElements &drawable) {
        drawable.type = DrawType::TRIANGLE_STRIP;
        drawable.strips = 1;
        drawable.verticesPerStrip = this->indices.count;
        this->initDrawable(drawable);
    }

}