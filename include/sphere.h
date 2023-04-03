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
    struct SphereVertices final {
        T* vertices;
        int count = 0;

        inline size_t size() const { return sizeof(T) * count; }
        inline float* to_float() const { return (float*) vertices; }
    };

    template<typename T>
    struct Sphere final {
        SphereVertices<T> vertices;
        u32* indices;
        int x_segments = 64;
        int y_segments = 64;

        void init(DrawableElements& drawable);

        void generate(
                DrawableElements& drawable,
                const std::function<void(T&, const glm::vec3&, const glm::vec2&)>& init_function
        );

        void init_default(DrawableElements& drawable);

        void init_tbn(DrawableElements& drawable);

        void displace(
                DrawableElements& drawable,
                const char* displacement_filepath,
                bool flip_uv = false,
                float scale = 1.0f,
                float shift = 0.0f,
                const std::function<void(T&)>& update_vertex_function = [](T& t) {}
        );

        void displace_tbn(
                DrawableElements& drawable,
                const char* displacement_path,
                bool flip_uv = false,
                float scale = 1.0f,
                float shift = 0.0f
        );
    };

    typedef Sphere<VertexDefault> SphereDefault;
    typedef Sphere<VertexUV> SphereUV;
    typedef Sphere<VertexTBN> SphereTBN;

    template<typename T>
    void Sphere<T>::init(DrawableElements& drawable)
    {
        drawable.type = GL_TRIANGLE_STRIP;
        drawable.vao.init();
        drawable.vao.bind();
        drawable.vbo.init(vertices, T::format, GL_DYNAMIC_DRAW);
        drawable.ibo.init(indices, drawable.index_count, GL_DYNAMIC_DRAW);
    }

    template<typename T>
    void Sphere<T>::generate(
            DrawableElements& drawable,
            const std::function<void(T&, const glm::vec3&, const glm::vec2&)>& init_function
    ) {
        vertices.count = (x_segments + 1) * (y_segments + 1);
        vertices.vertices = new T[vertices.count];

        int i = 0;
        for (u32 x = 0; x <= x_segments; x++) {
            for (u32 y = 0; y <= y_segments; y++) {
                T& V = vertices.vertices[i];

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

        this->indices = new u32[indices.size()];
        memcpy(this->indices, indices.data(), indices.size() * sizeof(u32));
        drawable.index_count = (int) indices.size();
    }

    template<typename T>
    void Sphere<T>::displace(
            DrawableElements& drawable,
            const char* displacement_filepath,
            bool flip_uv,
            float scale,
            float shift,
            const std::function<void(T&)>& update_vertex_function
    ) {
        // todo consider to move image reading and pass image data instead
        io::ImageData image = io::read_image(displacement_filepath, flip_uv);
        int image_width = image.width;
        int w = x_segments < image_width ? x_segments : image_width - 1;
        int h = y_segments < image.height ? y_segments : image.height - 1;
        int channels = image.channels;
        u8* data = image.data;

        int k = 0;
        for (int i = 0; i <= h; i++)
        {
            for (int j = 0; j <= w; j++)
            {
                auto& V = vertices.vertices[k];
                // retrieve texel for (i, j) uv
                u8* texel = data + (j + image_width * i) * channels;
                // raw height at coordinate
                float y = texel[0];
                y = y * scale + shift;
                y /= (float) image_width;
                // displace vertex height position
                V.pos.y += y;
                update_vertex_function(V);
                k++;
            }
        }

        drawable.vao.bind();
        drawable.vbo.update(vertices);

        io::free(image.data);
    }

    template<typename T>
    void Sphere<T>::displace_tbn(
            DrawableElements &drawable,
            const char *displacement_path,
            bool flip_uv,
            float scale, float shift
    ) {
        displace(drawable, displacement_path, flip_uv, scale, shift, [](T& V) {
            V.normal = V.pos;

            glm::vec3 right = { 1, 0, 0 };
            glm::vec3 up = { 0, 1, 0 };

            if (glm::dot(up, V.normal) == 1) {
                V.tangent = right;
            } else {
                V.tangent = glm::normalize(glm::cross(up, V.normal));
            }
        });
    }

    template<typename T>
    void Sphere<T>::init_default(DrawableElements &drawable) {
        generate(drawable, [](T& V, const glm::vec3& pos, const glm::vec2& uv) {
            V.pos = pos;
        });
        init(drawable);
    }

    template<typename T>
    void Sphere<T>::init_tbn(DrawableElements &drawable) {
        generate(drawable, [](T& V, const glm::vec3& pos, const glm::vec2& uv) {
            V.pos = pos;
            V.uv = uv;
            V.normal = pos;

            glm::vec3 right = { 1, 0, 0 };
            glm::vec3 up = { 0, 1, 0 };

            if (glm::dot(up, V.normal) == 1) {
                V.tangent = right;
            } else {
                V.tangent = glm::normalize(glm::cross(up, V.normal));
            }
        });
        init(drawable);
    }

}