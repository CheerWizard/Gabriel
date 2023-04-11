#pragma once

#include <shader.h>
#include <buffers.h>
#include <draw.h>
#include <geometry.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct Sphere : Geometry<T> {
        int x_segments = 64;
        int y_segments = 64;

        Sphere() {
            generate();
        }

        Sphere(int x_segments, int y_segments) : x_segments(x_segments), y_segments(y_segments) {
            generate();
        }

        void init(DrawableElements& drawable);

    private:
        void generate();
    };

    struct SphereUV : Sphere<VertexUV> {
        SphereUV();
        SphereUV(int x_segments, int y_segments);
    };

    struct SphereTBN : Sphere<VertexTBN> {
        SphereTBN();
        SphereTBN(int x_segments, int y_segments);

    private:
        void generate_tbn();
    };

    template<typename T>
    void Sphere<T>::generate() {
        this->vertices.init((x_segments + 1) * (y_segments + 1));

        int i = 0;
        for (u32 x = 0; x <= x_segments; x++) {
            for (u32 y = 0; y <= y_segments; y++) {
                T& V = this->vertices[i];

                float xSegment = (float)x / (float)x_segments;
                float ySegment = (float)y / (float)y_segments;

                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                V.pos = { xPos, yPos, zPos };
                V.uv = { xSegment, ySegment };

                i++;
            }
        }

        bool oddRow = false;
        Indices new_indices;
        std::vector<u32> index_list;

        for (u32 y = 0; y < y_segments; y++) {
            if (!oddRow) {
                for (u32 x = 0; x <= x_segments; x++) {
                    index_list.push_back(y * (x_segments + 1) + x);
                    index_list.push_back((y + 1) * (x_segments + 1) + x);
                }
            }
            else {
                for (int x = x_segments; x >= 0; x--) {
                    index_list.push_back((y + 1) * (x_segments + 1) + x);
                    index_list.push_back(y * (x_segments + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        new_indices.indices = index_list.data();
        new_indices.count = (int) index_list.size();
        this->indices.copy_from(&new_indices);
    }

    template<typename T>
    void Sphere<T>::init(DrawableElements &drawable) {
        drawable.type = GL_TRIANGLE_STRIP;
        drawable.strips = 1;
        drawable.vertices_per_strip = this->indices.count;
        this->init_drawable(drawable);
    }

}