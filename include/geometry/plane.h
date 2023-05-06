#pragma once

#include <api/buffers.h>

#include <geometry/geometry.h>

#include <functional>

namespace gl {

    template<typename T>
    struct Plane : Geometry<T> {
        int size = 64;

        Plane() = default;

        Plane(int size) : size(size) {
            generate();
        }

        void generate();
        void init(DrawableElements& drawable);
    };

    struct PlaneUV : Plane<VertexUV> {
        PlaneUV() = default;
        PlaneUV(int size) : Plane<VertexUV>(size) {}
    };

    struct PlaneTBN : Plane<VertexTBN> {
        PlaneTBN() = default;
        PlaneTBN(int size) : Plane<VertexTBN>(size) {
            generateTBN();
        }

        void generateTBN();
    };

    template<typename T>
    void Plane<T>::init(DrawableElements &drawable) {
        drawable.type = GL_TRIANGLES;
        drawable.strips = 1;
        drawable.verticesPerStrip = this->indices.count;
        this->initDrawable(drawable);
    }

    template<typename T>
    void Plane<T>::generate() {
        this->vertices.init(size * size);
        float shiftX = -size / 2.0f;
        float shiftZ = shiftX;
        int k = 0;

        // TODO: try to speed-up vertices generation time
        for (int i = 0 ; i < size ; i++) {
            for (int j = 0 ; j < size ; j++) {
                T& v = this->vertices[k];
                v.pos = { shiftX + i, 0, shiftZ + j };
                v.uv = { (float) i / (float) size, (float) j / (float) size };
                k++;
            }
        }

        this->indices.init((size - 1) * (size - 1) * 6);
        k = 0;

        // TODO: try to speed-up indices generation time
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - 1; j++) {
                u32 bottomLeft = i * size + j;
                u32 bottomRight = i * size + j + 1;
                u32 topLeft = (i + 1) * size + j;
                u32 topRight = (i + 1) * size + j + 1;
                // top-left triangle
                this->indices[k++] = bottomLeft;
                this->indices[k++] = topLeft;
                this->indices[k++] = topRight;
                // bottom-right triangle
                this->indices[k++] = bottomLeft;
                this->indices[k++] = topRight;
                this->indices[k++] = bottomRight;
            }
        }
    }

}