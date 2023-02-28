#pragma once

#include <shader.h>
#include <buffers.h>
#include <draw.h>

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
    typedef rect<vertex_tbn> rect_tbn;

    template<typename T>
    void rect_init(drawable_elements& drawable, const rect<T>& rect)
    {
        drawable.vao = vao_init();
        vao_bind(drawable.vao);
        drawable.vbo = vbo_init(rect.vertices, T::format, GL_STATIC_DRAW);
        drawable.ibo = ibo_init(rect.indices, 6, GL_STATIC_DRAW);
        drawable.index_count = 6;
    }

    template<typename T>
    std::vector<rect<T>> rects_init(
            drawable_elements& drawable,
            u32 count,
            const std::function<rect<T>(u32)>& rect_factory_fn = [](u32 i) { return rect<T>(); })
    {
        std::vector<rect_vertices<T>> vertices;
        std::vector<u32> indices;
        std::vector<rect<T>> rects;

        drawable.vao = vao_init();
        vao_bind(drawable.vao);
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
        drawable.vbo = vbo_init(vertices, T::format, GL_STATIC_DRAW);
        drawable.ibo = ibo_init(indices.data(), indices.size(), GL_STATIC_DRAW);

        return rects;
    }

    void rect_uv_init(drawable_elements& drawable);
    void rect_uv_normal_init(drawable_elements& drawable);
    void rect_tbn_init(drawable_elements& drawable);

    constexpr auto rect_default_init = &rect_init<vertex_default>;

    constexpr auto rect_solid_init = &rect_init<vertex_solid>;
    constexpr auto rect_solid_normal_init = &rect_init<vertex_solid_normal>;

    constexpr auto rects_solid_init = &rects_init<vertex_solid>;
    constexpr auto rects_solid_normal_init = &rects_init<vertex_solid_normal>;
    constexpr auto rects_uv_init = &rects_init<vertex_uv>;
    constexpr auto rects_uv_normal_init = &rects_init<vertex_uv_normal>;

}