#pragma once

#include <shader.h>
#include <buffers.h>

#include <gtc/type_ptr.hpp>

#include <vector>
#include <functional>

namespace gl {

    template<typename T>
    struct triangle {
        T v0 = { { -0.5f, -0.5f, 0 } };
        T v1 = { { 0.5f, -0.5f, 0 } };
        T v2 = { { 0, 0.5f, 0 } };

        inline float* to_float() const { return (float*) &v0.pos.x; }
    };

    typedef triangle<vertex_solid> triangle_solid;
    typedef triangle<vertex_uv> triangle_uv;
    typedef triangle<vertex_solid_normal> triangle_solid_normal;
    typedef triangle<vertex_uv_normal> triangle_uv_normal;

    template<typename T>
    void triangle_init(u32 vao, u32& vbo, const triangle<T>& triangle)
    {
        vao_bind(vao);
        vbo_init(triangle, T::format, GL_STATIC_DRAW);
    }

    template<typename T>
    std::vector<triangle<T>> triangles_init(
            u32 vao,
            u32& vbo,
            u32 count,
            const std::function<triangle<T>(u32)>& triangle_factory_fn = [](u32 i) { return triangle<T>(); }
    )
    {
        std::vector<triangle<T>> triangles;

        vao_bind(vao);
        triangles.reserve(count);
        for (u32 i = 0 ; i < count ; i++) {
            triangles.emplace_back(triangle_factory_fn(i));
        }
        vbo = vbo_init(triangles, T::format, GL_STATIC_DRAW);

        return triangles;
    }

    constexpr auto triangle_solid_init = &triangle_init<vertex_solid>;
    constexpr auto triangle_solid_normal_init = &triangle_init<vertex_solid_normal>;
    constexpr auto triangle_uv_init = &triangle_init<vertex_uv>;
    constexpr auto triangle_uv_normal_init = &triangle_init<vertex_uv_normal>;

    constexpr auto triangles_solid_init = &triangles_init<vertex_solid>;
    constexpr auto triangles_solid_normal_init = &triangles_init<vertex_solid_normal>;
    constexpr auto triangles_uv_init = &triangles_init<vertex_uv>;
    constexpr auto triangles_uv_normal_init = &triangles_init<vertex_uv_normal>;

}