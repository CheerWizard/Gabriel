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
    struct sphere_vertices final {
        int count = 0;
        T* vertices;

        inline size_t size() const { return sizeof(T) * count; }
        inline float* to_float() const { return (float*) &vertices[0].pos.x; }
    };

    template<typename T>
    struct sphere final {
        sphere_vertices<T> vertices;
        u32* indices;
    };

    typedef sphere<vertex_default> sphere_default;
    typedef sphere<vertex_solid> sphere_solid;
    typedef sphere<vertex_uv> sphere_uv;
    typedef sphere<vertex_solid_normal> sphere_solid_normal;
    typedef sphere<vertex_uv_normal> sphere_uv_normal;
    typedef sphere<vertex_tbn> sphere_tbn;

    template<typename T>
    void sphere_init(drawable_elements& drawable, const sphere<T>& sphere)
    {
        drawable.type = GL_TRIANGLE_STRIP;
        drawable.vao = vao_init();
        vao_bind(drawable.vao);
        drawable.vbo = vbo_init(sphere.vertices, T::format, GL_STATIC_DRAW);
        drawable.ibo = ibo_init(sphere.indices, drawable.index_count, GL_STATIC_DRAW);
    }

    template<typename T>
    sphere<T> sphere_init(
            drawable_elements& drawable,
            const std::function<void(T&, const glm::vec3&, const glm::vec2&)>& init_function,
            int x_segments = 64,
            int y_segments = 64
    ) {
        gl::sphere<T> sphere;
        int vertex_count = (x_segments + 1) * (y_segments + 1);
        sphere.vertices.vertices = new T[vertex_count];
        sphere.vertices.count = vertex_count;

        int i = 0;
        auto& vertices = sphere.vertices.vertices;
        for (u32 x = 0; x <= x_segments; x++) {
            for (u32 y = 0; y <= y_segments; y++) {
                auto& V = vertices[i];

                float xSegment = (float)x / (float)x_segments;
                float ySegment = (float)y / (float)y_segments;

                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                init_function(V, { xPos, yPos, zPos }, { xSegment, ySegment });

                i++;
            }
        }

        bool oddRow = false;
        std::vector<u32> indices;
        for (u32 y = 0; y < y_segments; y++) {
            if (!oddRow) {
                for (u32 x = 0; x <= x_segments; x++) {
                    indices.push_back(y * (x_segments + 1) + x);
                    indices.push_back((y + 1) * (x_segments + 1) + x);
                }
            }
            else {
                for (int x = x_segments; x >= 0; x--) {
                    indices.push_back((y + 1) * (x_segments + 1) + x);
                    indices.push_back(y * (x_segments + 1) + x);
                }
            }
            oddRow = !oddRow;
        }

        sphere.indices = new u32[indices.size()];
        memcpy(sphere.indices, indices.data(), indices.size());

        drawable.index_count = (int) indices.size();

        return sphere;
    }

    sphere_default sphere_default_init(drawable_elements& drawable, int x_segments = 64, int y_segments = 64);
    sphere_tbn sphere_tbn_init(drawable_elements& drawable, int x_segments = 64, int y_segments = 64);

}