#pragma once

#include <texture.h>
#include <vector>

namespace gl {

    struct ColorAttachment final {
        u32 index = 0;
        gl::Texture view;
        gl::TextureParams params;
        gl::TextureData data;

        ColorAttachment() = default;

        ColorAttachment(u32 index) : index(index) {}

        ColorAttachment(u32 index, int w, int h) : index(index) {
            data.width = w;
            data.height = h;
        }

        ColorAttachment(u32 index, int w, int h, int s) : index(index) {
            data.width = w;
            data.height = h;
            data.samples = s;
        }

        void init();
        void free();

        void attach() const;
    };

    struct DepthAttachment final {
        gl::Texture view;
        gl::TextureParams params;
        gl::TextureData data;

        void init();
        void free();

        void attach();
    };

    struct DepthStencilAttachment final {
        TextureData data;
        Texture view;

        void init();
        void free();

        void attach();
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

        void init();
        void free();

        void attach() const;

        void update();
    };

    struct FrameBuffer final {
        u32 id = 0;
        std::vector<ColorAttachment> colors;
        DepthAttachment depth;
        DepthStencilAttachment depth_stencil;
        RenderBuffer rbo;

        void init();
        void init_with_render_buffer();
        void free();

        void bind();
        static void unbind();

        void init_colors();
        void free_colors();

        void attach_colors();
        void attach_depth();
        void attach_depth_stencil();
        void attach_render_buffer();

        void complete();

        void resize(int w, int h);

        template<typename T>
        T read_pixel(int attachment_index, int x, int y);

        static void blit(
                u32 in_fbo, int in_w, int in_h,
                u32 out_fbo, int out_w, int out_h,
                int buffers_size = 1,
                int buffer_bit = GL_COLOR_BUFFER_BIT,
                int filter = GL_NEAREST
        );

        static void blit(
                u32 in_fbo, int in_w, int in_h, int in_attachment,
                u32 out_fbo, int out_w, int out_h, int out_attachment,
                int filter = GL_NEAREST
        );
    };

    template<typename T>
    T FrameBuffer::read_pixel(int attachment_index, int x, int y) {
        bind();

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);

        T pixel;
        glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);

        glReadBuffer(GL_NONE);

        unbind();

        return pixel;
    }

}