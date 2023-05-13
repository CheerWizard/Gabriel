#pragma once

#include <api/shader.h>
#include <api/buffers.h>
#include <api/draw.h>

#include <geometry/geometry.h>

#include <math/maths.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct Sphere : Geometry<T> {
        int xSegments = 64;
        int ySegments = 64;

        Sphere() = default;

        Sphere(int xSegments, int ySegments) : xSegments(xSegments), ySegments(ySegments) {
            generate();
        }

        void generate();
        void init(DrawableElements& drawable);
    };

    struct SphereUV : Sphere<VertexUV> {
        SphereUV() = default;
        SphereUV(int xSegments, int ySegments) : Sphere<VertexUV>(xSegments, ySegments) {}
    };

    struct SphereTBN : Sphere<VertexTBN> {
        SphereTBN() = default;
        SphereTBN(int xSegments, int ySegments) : Sphere<VertexTBN>(xSegments, ySegments) {
            generateTBN();
        }

        void generateTBN();
    };

    template<typename T>
    void Sphere<T>::generate() {
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
    void Sphere<T>::init(DrawableElements &drawable) {
        drawable.type = DrawType::TRIANGLE_STRIP;
        drawable.strips = 1;
        drawable.verticesPerStrip = this->indices.count;
        this->initDrawable(drawable);
    }

}