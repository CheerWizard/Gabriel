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
    struct cube_vertices final {

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

        inline size_t size() const { return sizeof(cube_vertices<T>); }
        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    template<typename T>
    struct cube final {
        cube_vertices<T> vertices;
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
    };

    typedef cube<vertex_solid> cube_solid;
    typedef cube<vertex_uv> cube_uv;
    typedef cube<vertex_solid_normal> cube_solid_normal;
    typedef cube<vertex_uv_normal> cube_uv_normal;
    typedef cube<vertex_tbn> cube_tbn;

    template<typename T>
    void cube_init(drawable_elements& drawable, const cube<T>& cube)
    {
        drawable.vao = vao_init();
        vao_bind(drawable.vao);
        drawable.vbo = vbo_init(cube.vertices, T::format, GL_STATIC_DRAW);
        drawable.ibo = ibo_init(cube.indices, 36, GL_STATIC_DRAW);
        drawable.index_count = 36;
    }

    template<typename T>
    std::vector<cube<T>> cubes_init(
            u32 vao,
            u32& vbo,
            u32& ibo,
            u32 count,
            const std::function<cube<T>(u32)>& cube_factory_fn = [](u32 i) { return cube<T>(); })
    {
        std::vector<cube_vertices<T>> vertices;
        std::vector<u32> indices;
        std::vector<cube<T>> cubes;

        vao_bind(vao);

        vertices.reserve(count * 24);
        indices.reserve(count * 36);
        int index_offset = 0;
        for (u32 i = 0 ; i < count ; i++) {
            cube<T> new_cube = cube_factory_fn(i);
            vertices.emplace_back(new_cube.vertices);
            for (u32& index : new_cube.indices) {
                indices.emplace_back(index + index_offset);
            }
            cubes.emplace_back(new_cube);
            index_offset += 24;
        }
        vbo = vbo_init(vertices, T::format, GL_STATIC_DRAW);
        ibo = ibo_init(indices.data(), indices.size(), GL_STATIC_DRAW);

        return cubes;
    }

    cube_uv cube_uv_init(drawable_elements& drawable);
    cube_uv_normal cube_uv_normal_init(drawable_elements& drawable);
    cube_solid cube_solid_init(drawable_elements& drawable, const glm::vec4& fill_color);
    cube_tbn cube_tbn_init(drawable_elements& drawable);

    constexpr auto cube_solid_normal_init = &cube_init<vertex_solid_normal>;
    constexpr auto cube_default_init = &cube_init<vertex_default>;

    constexpr auto cubes_solid_init = &cubes_init<vertex_solid>;
    constexpr auto cubes_solid_normal_init = &cubes_init<vertex_solid_normal>;
    constexpr auto cubes_uv_init = &cubes_init<vertex_uv>;
    constexpr auto cubes_uv_normal_init = &cubes_init<vertex_uv_normal>;

}