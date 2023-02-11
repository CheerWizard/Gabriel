#pragma once

#include <vertex.h>
#include <glad/glad.h>

#include <vector>

namespace gl {

    u32 vao_init();
    void vao_bind(u32 vao);
    void vao_free(u32 vao);

    void vbo_bind(u32 vbo);
    void vbo_free(u32 vbo);
    void vbo_set_format(const vertex_format& format);

    template<typename T>
    u32 vbo_init(const T& geometry, const vertex_format& format, int alloc_type) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        vertex_data_t vertex_data {};
        vertex_data.size = sizeof(T);
        vertex_data.data = geometry.to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        vbo_set_format(format);

        return id;
    }

    template<typename T>
    u32 vbo_init(const std::vector<T>& geometries, const vertex_format& format, int alloc_type) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        vertex_data_t vertex_data {};
        vertex_data.size = sizeof(T) * geometries.size();
        vertex_data.data = geometries.begin()->to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        vbo_set_format(format);

        return id;
    }

    template<typename T>
    void vbo_update(u32 vbo, const T& geometry) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        vertex_data_t vertex_data {};
        vertex_data.size = sizeof(T);
        vertex_data.data = geometry.to_float();
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data.size, vertex_data.data);
    }

    void ibo_bind(u32 ibo);
    void ibo_free(u32 ibo);
    u32 ibo_init(const u32 indices[], u32 index_count, int alloc_type);
}
