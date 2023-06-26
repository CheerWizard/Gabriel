#pragma once

#include <api/image.h>
#include <api/viewport.h>

namespace gl {

    struct GABRIEL_API ColorAttachment final {
        u32 index = 0;
        gl::ImageBuffer buffer;
        gl::ImageParams params;
        gl::Image image;
        glm::vec4 clearColor = COLOR_CLEAR;

        ColorAttachment() = default;

        ColorAttachment(u32 index) : index(index) {}

        ColorAttachment(u32 index, int w, int h) : index(index) {
            image.width = w;
            image.height = h;
        }

        ColorAttachment(u32 index, int w, int h, int s) : index(index) {
            image.width = w;
            image.height = h;
            image.samples = s;
        }

        void init();
        void free();

        void attach() const;

        void resize(int w, int h);

        void clearBuffer() const;
    };

    struct GABRIEL_API DepthAttachment final {
        gl::ImageBuffer buffer;
        gl::ImageParams params;
        gl::Image image;
        glm::vec4 clearColor = COLOR_CLEAR;

        void init();
        void free();

        void attach();

        void clearBuffer() const;
    };

    struct GABRIEL_API DepthStencilAttachment final {
        ImageBuffer buffer;
        Image image;
        glm::vec4 clearColor = COLOR_CLEAR;

        void init();
        void free();

        void attach();

        void clearBuffer() const;
    };

    struct GABRIEL_API RenderBuffer final {
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

    struct GABRIEL_API FrameBuffer final {
        u32 id = 0;
        std::vector<ColorAttachment> colors;
        DepthAttachment depth;
        DepthStencilAttachment depthStencil;
        RenderBuffer rbo;
        glm::vec4 clearColor = COLOR_CLEAR;

        void init();
        void initWithRenderBuffer();
        void free();

        void clearColorBuffers() const;
        void clearDepthBuffer() const;
        void clearStencilBuffer() const;
        static void clearBuffer(const glm::vec4& color = COLOR_CLEAR, int bitmask = GL_COLOR_BUFFER_BIT);
        static void clearBuffer(int bitmask);

        void bind() const;
        void bindWriting() const;
        void bindReading() const;
        static void unbind();

        void initColors();
        void freeColors();

        void attachColors();
        void attachDepth();
        void attachDepthStencil();
        void attachRenderBuffer();

        void complete();

        void resize(int w, int h);

        void resample(int samples);

        template<typename T>
        void readPixel(T& pixel, int attachment_index, int x, int y);

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
    void FrameBuffer::readPixel(T& pixel, int attachment_index, int x, int y) {
        bind();

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);

        glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);

        glReadBuffer(GL_NONE);

        unbind();
    }

}