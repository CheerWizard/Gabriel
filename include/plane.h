#pragma once

#include <geometry.h>
#include <buffers.h>

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
            generate_tbn();
        }

        void generate_tbn();
    };

    template<typename T>
    void Plane<T>::init(DrawableElements &drawable) {
        drawable.type = GL_TRIANGLES;
        drawable.strips = 1;
        drawable.vertices_per_strip = this->indices.count;
        this->init_drawable(drawable);
    }

    template<typename T>
    void Plane<T>::generate() {
        this->vertices.init(size * size);
        float shift_x = -size / 2.0f;
        float shift_z = shift_x;
        int k = 0;

        // TODO: try to speed-up vertices generation time
        for (int i = 0 ; i < size ; i++) {
            for (int j = 0 ; j < size ; j++) {
                T& v = this->vertices[k];
                v.pos = { shift_x + i, 0, shift_z + j };
                v.uv = { (float) i / (float) size, (float) j / (float) size };
                k++;
            }
        }

        this->indices.init((size - 1) * (size - 1) * 6);
        k = 0;

        // TODO: try to speed-up indices generation time
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - 1; j++) {
                u32 bottom_left = i * size + j;
                u32 bottom_right = i * size + j + 1;
                u32 top_left = (i + 1) * size + j;
                u32 top_right = (i + 1) * size + j + 1;
                // top-left triangle
                this->indices[k++] = bottom_left;
                this->indices[k++] = top_left;
                this->indices[k++] = top_right;
                // bottom-right triangle
                this->indices[k++] = bottom_left;
                this->indices[k++] = top_right;
                this->indices[k++] = bottom_right;
            }
        }
    }

}