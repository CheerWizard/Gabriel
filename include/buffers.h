#pragma once

#include <vertex.h>
#include <texture.h>

#include <glad/glad.h>

#include <vector>

namespace gl {

    struct VertexArray final {
        u32 id;

        void init();
        void bind() const;
        void free();
        void draw_quad() const;
    };

    struct VertexBuffer final {
        u32 id;

        void init();
        template<typename T>
        void init(const T& geometry, const VertexFormat& format, int alloc_type);
        template<typename T>
        void init(const std::vector<T>& geometries, const VertexFormat& format, int alloc_type);

        void bind() const;

        static void set_format(const VertexFormat& format);
        template<typename T>
        void update(const T& geometry);

        void free();
    };

    template<typename T>
    void VertexBuffer::init(const T &geometry, const VertexFormat &format, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertex_data {};
        vertex_data.size = geometry.size();
        vertex_data.data = geometry.to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::init(const std::vector<T> &geometries, const VertexFormat &format, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ARRAY_BUFFER, id);
        VertexData vertex_data {};
        vertex_data.size = sizeof(T) * geometries.size();
        vertex_data.data = geometries.begin()->to_float();
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size, vertex_data.data, alloc_type);

        set_format(format);
    }

    template<typename T>
    void VertexBuffer::update(const T& geometry) {
        glBindBuffer(GL_ARRAY_BUFFER, id);

        VertexData vertex_data {};
        vertex_data.size = geometry.size();
        vertex_data.data = geometry.to_float();
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data.size, vertex_data.data);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    struct IndexBuffer final {
        u32 id;

        void init(u32* indices, u32 index_count, int alloc_type);
        void init(const u32* indices, u32 index_count, int alloc_type);

        void free();

        void bind() const;

        void update(u32* indices, u32 index_count);
        void update(const u32* indices, u32 index_count);
    };

    struct ColorAttachment final {
        gl::Texture view;
        gl::TextureParams params;
        gl::TextureData data;

        ColorAttachment() = default;

        ColorAttachment(int w, int h) {
            data.width = w;
            data.height = h;
        }

        ColorAttachment(int w, int h, int s) {
            data.width = w;
            data.height = h;
            data.samples = s;
        }

        void free();
    };

    struct DepthAttachment final {
        gl::Texture view;
        gl::TextureParams params;
        gl::TextureData data;

        void free();
    };

    struct DepthStencilAttachment final {
        int width = 800;
        int height = 600;
        int samples = 1;
        u32 primitive_type = GL_UNSIGNED_INT_24_8;
        void* data = null;
        Texture view;

        void free();
    };

    struct RenderBuffer final {
        int width = 800;
        int height = 600;
        int format = GL_DEPTH24_STENCIL8;
        int type = GL_DEPTH_STENCIL_ATTACHMENT;
        int samples = 1;
        u32 id = 0;

        RenderBuffer() = default;

        RenderBuffer(int w, int h) : width(w), height(h) {}

        RenderBuffer(int w, int h, int s) : width(w), height(h), samples(s) {}

        void free();

        void update();
    };

    enum FrameBufferFlags : u8 {
        init_colors = 1,
        init_depth = 2,
        init_depth_stencil = 4,
        init_render_buffer = 8,
        init_default = init_colors,
    };

    struct FrameBuffer final {
        u32 id = 0;
        std::vector<ColorAttachment> colors;
        DepthAttachment depth;
        DepthStencilAttachment depth_stencil;
        RenderBuffer rbo;
        u8 flags = init_default;

        void init();
        void init_with_render_buffer();
        void free();

        void bind();
        static void unbind();

        void free_colors();

        void attach_colors();
        void attach_depth();
        void attach_depth_stencil();
        void attach_render_buffer();

        void resize(int w, int h);

        static void blit(
                u32 in_fbo, int in_w, int in_h,
                u32 out_fbo, int out_w, int out_h,
                int buffers_size = 1,
                int buffer_bit = GL_COLOR_BUFFER_BIT,
                int filter = GL_NEAREST
        );
    };

    struct UniformData final {
        long long offset;
        long long size;
        void* data;
    };

    struct UniformBuffer final {
        u32 id;

        void init(u32 binding, long long size);
        void bind();
        void update(const UniformData& ubo_data);
        void free();
    };
}
