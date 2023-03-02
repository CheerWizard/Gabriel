#pragma once

#include <vertex.h>
#include <texture.h>

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
        vertex_data.size = geometry.size();
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
    u32 ibo_init(u32* indices, u32 index_count, int alloc_type);
    u32 ibo_init(const u32* indices, u32 index_count, int alloc_type);

    struct color_attachment final {
        gl::texture view;
        gl::texture_params params;
        gl::texture_data data;

        color_attachment() = default;

        color_attachment(int w, int h) {
            data.width = w;
            data.height = h;
        }

        color_attachment(int w, int h, int s) {
            data.width = w;
            data.height = h;
            data.samples = s;
        }
    };

    struct depth_attachment final {
        gl::texture view;
        gl::texture_params params;
        gl::texture_data data;
    };

    struct depth_stencil_attachment final {
        int width = 800;
        int height = 600;
        int samples = 1;
        u32 primitive_type = GL_UNSIGNED_INT_24_8;
        void* data = null;
        u32 id = 0;
        u32 type = GL_TEXTURE_2D;
    };

    struct render_buffer final {
        int width = 800;
        int height = 600;
        int format = GL_DEPTH24_STENCIL8;
        int type = GL_DEPTH_STENCIL_ATTACHMENT;
        int samples = 1;
        u32 id = 0;

        render_buffer() = default;

        render_buffer(int w, int h) : width(w), height(h) {}

        render_buffer(int w, int h, int s) : width(w), height(h), samples(s) {}
    };

    u32 fbo_init(
            std::vector<color_attachment>* colors,
            depth_attachment* depth,
            depth_stencil_attachment* depth_stencil,
            render_buffer* rbo
    );
    u32 fbo_init(render_buffer& rbo);
    void fbo_free(u32 fbo);

    void fbo_bind(u32 fbo);
    void fbo_unbind();

    void fbo_update(const render_buffer& rbo);

    void fbo_free_attachment(color_attachment& color);
    void fbo_free_attachment(std::vector<color_attachment>& colors);
    void fbo_free_attachment(depth_attachment& depth);
    void fbo_free_attachment(depth_stencil_attachment& depth_stencil);
    void fbo_free_attachment(render_buffer& rbo);

    void fbo_blit(
            u32 in_fbo, int in_w, int in_h,
            u32 out_fbo, int out_w, int out_h,
            int buffers_size = 1,
            int buffer_bit = GL_COLOR_BUFFER_BIT,
            int filter = GL_NEAREST
    );

    struct ubo_data final {
        long long offset;
        long long size;
        void* data;
    };

    u32 ubo_init(u32 binding, long long size);
    void ubo_bind(u32 ubo);
    void ubo_update(u32 ubo, const ubo_data& ubo_data);
    void ubo_update(const ubo_data& ubo_data);
    void ubo_free(u32 ubo);
}
