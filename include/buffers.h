#pragma once

#include <vertex.h>
#include <glad/glad.h>

#include <vector>

namespace gl {

    template<typename T>
    u32 static_vbo_init(const T& geometry, const vertex_format& format) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        vertex_data_t vertex_data {};
        vertex_data.size = sizeof(T);
        vertex_data.data = geometry.to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, GL_STATIC_DRAW);

        vbo_set_format(format);

        return id;
    }

    template<typename T>
    u32 static_vbo_init(const std::vector<T>& geometries, const vertex_format& format) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        vertex_data_t vertex_data {};
        vertex_data.size = sizeof(T) * geometries.size();
        vertex_data.data = geometries.begin()->to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, GL_STATIC_DRAW);

        vbo_set_format(format);

        return id;
    }

    void vbo_set_format(const vertex_format& format);
    void vbo_bind(u32 vbo);
    void vbo_free(u32 vbo);

    u32 static_ibo_init(const u32 indices[], u32 index_count);
    void ibo_bind(u32 ibo);
    void ibo_free(u32 ibo);

    u32 vao_init();
    void vao_bind(u32 vao);
    void vao_free(u32 vao);

}
