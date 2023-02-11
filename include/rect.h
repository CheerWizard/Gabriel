#pragma once

#include <shader.h>
#include <buffers.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct rect_vertices final {
        T v0 = { { 0.5f, 0.5f, 0 } };
        T v1 = { { 0.5f, -0.5f, 0 } };
        T v2 = { { -0.5f, -0.5f, 0 } };
        T v3 = { { -0.5, 0.5f, 0 } };

        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    template<typename T>
    struct rect final {
        rect_vertices<T> vertices;
        u32 indices[6] = {
                0, 1, 3,
                1, 2, 3
        };
    };

    typedef rect<vertex_solid> rect_solid;
    typedef rect<vertex_uv> rect_uv;
    typedef rect<vertex_solid_normal> rect_solid_normal;
    typedef rect<vertex_uv_normal> rect_uv_normal;

    template<typename T>
    void rect_init(u32& shader_program, u32& vao, u32& vbo, u32& ibo, const rect<T>& rect)
    {
        shader_program = shader_init(T::shader_props);
        vao = vao_init();
        vao_bind(vao);
        vbo = vbo_init(rect.vertices, T::format, GL_STATIC_DRAW);
        ibo = ibo_init(rect.indices, 6, GL_STATIC_DRAW);
    }

    template<typename T>
    std::vector<rect<T>> rects_init(
            u32& shader_program,
            u32& vao,
            u32& vbo,
            u32& ibo,
            u32 count,
            const std::function<rect<T>(u32)>& rect_factory_fn = [](u32 i) { return rect<T>(); })
    {
        std::vector<rect_vertices<T>> vertices;
        std::vector<u32> indices;
        std::vector<rect<T>> rects;

        shader_program = shader_init(T::shader_props);

        vao = vao_init();
        vao_bind(vao);

        vertices.reserve(count * 4);
        indices.reserve(count * 6);
        int index_offset = 0;
        for (u32 i = 0 ; i < count ; i++) {
            rect<T> new_rect = rect_factory_fn(i);
            vertices.emplace_back(new_rect.vertices);
            for (u32& index : new_rect.indices) {
                indices.emplace_back(index + index_offset);
            }
            rects.emplace_back(new_rect);
            index_offset += 4;
        }
        vbo = vbo_init(vertices, T::format, GL_STATIC_DRAW);
        ibo = ibo_init(indices.data(), indices.size(), GL_STATIC_DRAW);

        return rects;
    }

    constexpr auto rect_solid_init = &rect_init<vertex_solid>;
    constexpr auto rect_solid_normal_init = &rect_init<vertex_solid_normal>;
    constexpr auto rect_uv_init = &rect_init<vertex_uv>;
    constexpr auto rect_uv_normal_init = &rect_init<vertex_uv_normal>;

    constexpr auto rects_solid_init = &rects_init<vertex_solid>;
    constexpr auto rects_solid_normal_init = &rects_init<vertex_solid_normal>;
    constexpr auto rects_uv_init = &rects_init<vertex_uv>;
    constexpr auto rects_uv_normal_init = &rects_init<vertex_uv_normal>;

}