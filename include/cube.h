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
    struct CubeVertices final {

        T v0 = { { -0.5f,0.5f,-0.5f } };
        T v1 = { { -0.5f,-0.5f,-0.5f } };
        T v2 = { { 0.5f,-0.5f,-0.5f } };
        T v3 = { { 0.5f,0.5f,-0.5f } };

        T v4 = { { -0.5f, 0.5f, 0.5f } };
        T v5 = { { -0.5f,-0.5f,0.5f } };
        T v6 = { { 0.5f,-0.5f,0.5f } };
        T v7 = { { 0.5f,0.5f,0.5f } };

        T v8 = { { 0.5f,0.5f,-0.5f } };
        T v9 = { { 0.5f,-0.5f,-0.5f } };
        T v10 = { { 0.5f,-0.5f,0.5f } };
        T v11 = { { 0.5f,0.5f,0.5f } };

        T v12 = { { -0.5f,0.5f,-0.5f } };
        T v13 = { { -0.5f,-0.5f,-0.5f } };
        T v14 = { { -0.5f,-0.5f,0.5f } };
        T v15 = { { -0.5f,0.5f,0.5f } };

        T v16 = { { -0.5f,0.5f,0.5f } };
        T v17 = { { -0.5f,0.5f,-0.5f } };
        T v18 = { { 0.5f,0.5f,-0.5f } };
        T v19 = { { 0.5f,0.5f,0.5f } };

        T v20 = { { -0.5f,-0.5f,0.5f } };
        T v21 = { { -0.5f,-0.5f,-0.5f } };
        T v22 = { { 0.5f,-0.5f,-0.5f } };
        T v23 = { { 0.5f,-0.5f,0.5f } };

        inline size_t size() const { return sizeof(CubeVertices<T>); }
        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    template<typename T>
    static void init_uv(CubeVertices<T>& vertices) {
        vertices.v0.uv = { 0, 0 };
        vertices.v1.uv = { 0, 1 };
        vertices.v2.uv = { 1, 1 };
        vertices.v3.uv = { 1, 0 };

        vertices.v4.uv = { 0, 0 };
        vertices.v5.uv = { 0, 1 };
        vertices.v6.uv = { 1, 1 };
        vertices.v7.uv = { 1, 0 };

        vertices.v8.uv = { 0, 0 };
        vertices.v9.uv = { 0, 1 };
        vertices.v10.uv = { 1, 1 };
        vertices.v11.uv = { 1, 0 };

        vertices.v12.uv = { 0, 0 };
        vertices.v13.uv = { 0, 1 };
        vertices.v14.uv = { 1, 1 };
        vertices.v15.uv = { 1, 0 };

        vertices.v16.uv = { 0, 0 };
        vertices.v17.uv = { 0, 1 };
        vertices.v18.uv = { 1, 1 };
        vertices.v19.uv = { 1, 0 };

        vertices.v20.uv = { 0, 0 };
        vertices.v21.uv = { 0, 1 };
        vertices.v22.uv = { 1, 1 };
        vertices.v23.uv = { 1, 0 };
    }

    template<typename T>
    static void init_normal(CubeVertices<T>& vertices) {
        glm::vec3 normal1 = -glm::normalize(glm::cross(
                vertices.v1.pos - vertices.v0.pos,
                vertices.v3.pos - vertices.v0.pos
        ));

        glm::vec3 normal2 = glm::normalize(glm::cross(
                vertices.v5.pos - vertices.v4.pos,
                vertices.v7.pos - vertices.v4.pos
        ));

        glm::vec3 normal3 = -glm::normalize(glm::cross(
                vertices.v9.pos - vertices.v8.pos,
                vertices.v11.pos - vertices.v8.pos
        ));

        glm::vec3 normal4 = glm::normalize(glm::cross(
                vertices.v13.pos - vertices.v12.pos,
                vertices.v15.pos - vertices.v12.pos
        ));

        glm::vec3 normal5 = -glm::normalize(glm::cross(
                vertices.v17.pos - vertices.v16.pos,
                vertices.v19.pos - vertices.v16.pos
        ));

        glm::vec3 normal6 = glm::normalize(glm::cross(
                vertices.v21.pos - vertices.v20.pos,
                vertices.v23.pos - vertices.v20.pos
        ));

        vertices.v0.normal = normal1;
        vertices.v1.normal = normal1;
        vertices.v2.normal = normal1;
        vertices.v3.normal = normal1;

        vertices.v4.normal = normal2;
        vertices.v5.normal = normal2;
        vertices.v6.normal = normal2;
        vertices.v7.normal = normal2;

        vertices.v8.normal = normal3;
        vertices.v9.normal = normal3;
        vertices.v10.normal = normal3;
        vertices.v11.normal = normal3;

        vertices.v12.normal = normal4;
        vertices.v13.normal = normal4;
        vertices.v14.normal = normal4;
        vertices.v15.normal = normal4;

        vertices.v16.normal = normal5;
        vertices.v17.normal = normal5;
        vertices.v18.normal = normal5;
        vertices.v19.normal = normal5;

        vertices.v20.normal = normal6;
        vertices.v21.normal = normal6;
        vertices.v22.normal = normal6;
        vertices.v23.normal = normal6;
    }

    template<typename T>
    static void init_tbn(CubeVertices<T>& vertices) {
        init_tbn(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
        init_tbn(&vertices.v4, &vertices.v5, &vertices.v6, &vertices.v7);
        init_tbn(&vertices.v8, &vertices.v9, &vertices.v10, &vertices.v11);
        init_tbn(&vertices.v12, &vertices.v13, &vertices.v14, &vertices.v15);
        init_tbn(&vertices.v16, &vertices.v17, &vertices.v18, &vertices.v19);
        init_tbn(&vertices.v20, &vertices.v21, &vertices.v22, &vertices.v23);
    }

    template<typename T>
    struct Cube final {
        CubeVertices<T> vertices;
        u32 indices[36] = {
                0,1,3,
                3,1,2,
                4,5,7,
                7,5,6,
                8,9,11,
                11,9,10,
                12,13,15,
                15,13,14,
                16,17,19,
                19,17,18,
                20,21,23,
                23,21,22
        };

        void init(DrawableElements& drawable);
        void init_uv(DrawableElements& drawable);
        void init_uv_normal(DrawableElements& drawable);
        void init_solid(DrawableElements& drawable, const glm::vec4& fill_color);
        void init_tbn(DrawableElements& drawable);
    };

    typedef Cube<VertexDefault> CubeDefault;
    typedef Cube<VertexUV> CubeUV;
    typedef Cube<VertexTBN> CubeTBN;

    template<typename T>
    void Cube<T>::init(DrawableElements& drawable)
    {
        drawable.vao.init();
        drawable.vao.bind();
        drawable.vbo.init(vertices, T::format, BufferAllocType::STATIC);
        drawable.ibo.init(indices, 36, BufferAllocType::STATIC);
        drawable.strips = 1;
        drawable.vertices_per_strip = 36;
    }

    template<typename T>
    void Cube<T>::init_uv(DrawableElements &drawable) {
        gl::init_uv(vertices);
        init(drawable);
    }

    template<typename T>
    void Cube<T>::init_uv_normal(DrawableElements &drawable) {
        gl::init_uv(vertices);
        gl::init_normal(vertices);
        init(drawable);
    }

    template<typename T>
    void Cube<T>::init_solid(DrawableElements &drawable, const glm::vec4 &fill_color) {
        vertices.v0.color = fill_color;
        vertices.v1.color = fill_color;
        vertices.v2.color = fill_color;
        vertices.v3.color = fill_color;
        vertices.v4.color = fill_color;
        vertices.v5.color = fill_color;
        vertices.v6.color = fill_color;
        vertices.v7.color = fill_color;
        vertices.v8.color = fill_color;
        vertices.v9.color = fill_color;
        vertices.v10.color = fill_color;
        vertices.v11.color = fill_color;
        vertices.v12.color = fill_color;
        vertices.v13.color = fill_color;
        vertices.v14.color = fill_color;
        vertices.v15.color = fill_color;
        vertices.v16.color = fill_color;
        vertices.v17.color = fill_color;
        vertices.v18.color = fill_color;
        vertices.v19.color = fill_color;
        vertices.v20.color = fill_color;
        vertices.v21.color = fill_color;
        vertices.v22.color = fill_color;
        vertices.v23.color = fill_color;

        init(drawable);
    }

    template<typename T>
    void Cube<T>::init_tbn(DrawableElements &drawable) {
        gl::init_uv(vertices);
        gl::init_normal(vertices);
        gl::init_tbn(vertices);
        init(drawable);
    }

    template<typename T>
    std::vector<Cube<T>> cubes_init(
            DrawableElements& drawable,
            u32 count,
            const std::function<Cube<T>(u32)>& cube_factory_fn = [](u32 i) { return Cube<T>(); })
    {
        std::vector<CubeVertices<T>> vertices;
        std::vector<u32> indices;
        std::vector<Cube<T>> cubes;

        drawable.vao.bind();

        vertices.reserve(count * 24);
        indices.reserve(count * 36);
        int index_offset = 0;
        for (u32 i = 0 ; i < count ; i++) {
            Cube<T> new_cube = cube_factory_fn(i);
            vertices.emplace_back(new_cube.vertices);
            for (u32& index : new_cube.indices) {
                indices.emplace_back(index + index_offset);
            }
            cubes.emplace_back(new_cube);
            index_offset += 24;
        }
        drawable.vbo.init(vertices, T::format, BufferAllocType::STATIC);
        drawable.ibo.init(indices.data(), indices.size(), BufferAllocType::STATIC);

        return cubes;
    }

}