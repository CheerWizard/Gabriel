#include <frame.h>

namespace gl {

    void FrameBuffer::init() {
        glGenFramebuffers(1, &id);
        bind();
    }

    void FrameBuffer::attach_colors() {
        for (const auto& color : colors) {
            color.attach();
        }
    }

    void FrameBuffer::attach_depth() {
        depth.attach();
    }

    void FrameBuffer::attach_depth_stencil() {
        depth_stencil.attach();
    }

    void FrameBuffer::attach_render_buffer() {
        rbo.attach();
    }

    void FrameBuffer::init_with_render_buffer() {
        glGenFramebuffers(1, &id);
        bind();

        rbo.init();
        rbo.attach();

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            print_err("fbo_init(): Failed to complete framebuffer!");
            print_err("Error status=" << status);
        }

        unbind();
    }

    void FrameBuffer::complete() {
        if (colors.empty()) {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        } else {
            size_t colors_size = colors.size();
            u32* color_attachments = (u32*) calloc(colors_size, sizeof(u32));
            for (int i = 0 ; i < colors_size ; i++) {
                color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glDrawBuffers((int) colors_size, color_attachments);
        }

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            print_err("fbo_init(): Failed to complete framebuffer!");
            print_err("Error status=" << status);
        }

        unbind();
    }

    void FrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::free() {
        free_colors();
        depth.free();
        depth_stencil.free();
        rbo.free();
        glDeleteFramebuffers(1, &id);
    }

    void FrameBuffer::init_colors() {
        for (auto& color : colors) {
            color.init();
        }
    }

    void FrameBuffer::free_colors() {
        for (auto& color : colors) {
            color.free();
        }
    }

    void FrameBuffer::blit(
            u32 in_fbo, int in_w, int in_h,
            u32 out_fbo, int out_w, int out_h,
            int buffers_size, int buffer_bit, int filter
    ) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, in_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, out_fbo);
        u32* buffers = (u32*) calloc(buffers_size, sizeof(u32));
        for (int i = 0 ; i < buffers_size ; i++) {
            buffers[i] = GL_COLOR_ATTACHMENT0 + i;
            glReadBuffer(buffers[i]);
            glDrawBuffer(buffers[i]);
            glBlitFramebuffer(0, 0, in_w, in_h, 0, 0, out_w, out_h, buffer_bit, filter);
        }
        glDrawBuffers(buffers_size, buffers);
    }

    void FrameBuffer::blit(
            u32 in_fbo, int in_w, int in_h, int in_attachment,
            u32 out_fbo, int out_w, int out_h, int out_attachment,
            int filter
    ) {
        u32 read_buffer = GL_COLOR_ATTACHMENT0 + in_attachment;
        u32 draw_buffer = GL_COLOR_ATTACHMENT0 + out_attachment;
        glBindFramebuffer(GL_READ_FRAMEBUFFER, in_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, out_fbo);
        glReadBuffer(read_buffer);
        glDrawBuffer(draw_buffer);
        glBlitFramebuffer(0, 0, in_w, in_h, 0, 0, out_w, out_h, GL_COLOR_BUFFER_BIT, filter);
        glDrawBuffers(1, &draw_buffer);
    }

    void FrameBuffer::resize(int w, int h) {
        bind();

        int color_size = colors.size();
        for (int i = 0 ; i < color_size ; i++) {
            auto& color = colors[i];
            color.data.width = w;
            color.data.height = h;
            color.free();
            color.init();
            color.attach();
        }

        if (depth.view.id != invalid_texture) {
            depth.data.width = w;
            depth.data.height = h;
            depth.free();
            depth.init();
            depth.attach();
        }

        if (depth_stencil.view.id != invalid_texture) {
            depth_stencil.data.width = w;
            depth_stencil.data.height = h;
            depth_stencil.free();
            depth_stencil.init();
            depth_stencil.attach();
        }

        if (rbo.id != 0) {
            rbo.width = w;
            rbo.height = h;
            rbo.free();
            rbo.init();
            rbo.attach();
        }
    }

    void ColorAttachment::init() {
        if (view.type == GL_TEXTURE_CUBE_MAP) {
            glGenTextures(1, &view.id);
            glBindTexture(GL_TEXTURE_CUBE_MAP, view.id);

            for (int i = 0 ; i < 6 ; i++) {
                glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                        GL_COLOR_ATTACHMENT0 + i,
                        data.width, data.height, 0,
                        GL_COLOR_ATTACHMENT0 + i,
                        data.primitive_type,
                        data.data
                );
            }

            if (params.min_filter == GL_NEAREST_MIPMAP_NEAREST
                || params.min_filter == GL_NEAREST_MIPMAP_LINEAR
                || params.min_filter == GL_LINEAR_MIPMAP_NEAREST
                || params.min_filter == GL_LINEAR_MIPMAP_LINEAR
                    ) {
                view.generate_mipmaps(params);
            }

            view.update_params(params);
        }
        else {
            view.init(data, params, view.type);
        }
    }

    void ColorAttachment::free() {
        view.free();
    }

    void ColorAttachment::attach() const {
        if (view.type == GL_TEXTURE_CUBE_MAP) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, view.id, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, view.type, view.id, 0);
        }
    }

    void DepthAttachment::init() {
        data.internal_format = GL_DEPTH_COMPONENT;
        data.data_format = GL_DEPTH_COMPONENT;
        view.init(data, params, view.type);
    }

    void DepthAttachment::free() {
        view.free();
    }

    void DepthAttachment::attach() {
        if (view.type == GL_TEXTURE_CUBE_MAP) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, view.id, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, view.type, view.id, 0);
        }
    }

    void DepthStencilAttachment::init() {
        data.internal_format = GL_DEPTH24_STENCIL8;
        data.data_format = GL_DEPTH_STENCIL;
        data.primitive_type = GL_UNSIGNED_INT_24_8;
        view.init(data);
    }

    void DepthStencilAttachment::free() {
        view.free();
    }

    void DepthStencilAttachment::attach() {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, view.type, view.id, 0);
    }

    void RenderBuffer::init() {
        glGenRenderbuffers(1, &id);
        glBindRenderbuffer(GL_RENDERBUFFER, id);

        if (samples > 1) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void RenderBuffer::free() {
        glDeleteRenderbuffers(1, &id);
    }

    void RenderBuffer::attach() const {
        glBindRenderbuffer(GL_RENDERBUFFER, id);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, id);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void RenderBuffer::update() {
        glBindRenderbuffer(GL_RENDERBUFFER, id);
        if (samples > 1) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
        }
    }

}