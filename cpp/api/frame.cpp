#include <api/frame.h>

namespace gl {

    void FrameBuffer::init() {
        glGenFramebuffers(1, &id);
        bind();
    }

    void FrameBuffer::attachColors() {
        for (const auto& color : colors) {
            color.attach();
        }
    }

    void FrameBuffer::attachDepth() {
        depth.attach();
    }

    void FrameBuffer::attachDepthStencil() {
        depthStencil.attach();
    }

    void FrameBuffer::attachRenderBuffer() {
        rbo.attach();
    }

    void FrameBuffer::initWithRenderBuffer() {
        glGenFramebuffers(1, &id);
        bind();

        rbo.init();
        rbo.attach();

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            error("Failed to complete framebuffer. Error status={0}", status);
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
            error("Failed to complete framebuffer. Error status={0}", status);
        }

        unbind();
    }

    void FrameBuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    void FrameBuffer::bindWriting() const {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    }

    void FrameBuffer::bindReading() const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::free() {
        freeColors();
        depth.free();
        depthStencil.free();
        rbo.free();
        glDeleteFramebuffers(1, &id);
    }

    void FrameBuffer::initColors() {
        for (auto& color : colors) {
            color.init();
        }
    }

    void FrameBuffer::freeColors() {
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
            color.image.width = w;
            color.image.height = h;
            color.free();
            color.init();
            color.attach();
        }

        if (depth.buffer.id != InvalidImageBuffer) {
            depth.image.width = w;
            depth.image.height = h;
            depth.free();
            depth.init();
            depth.attach();
        }

        if (depthStencil.buffer.id != InvalidImageBuffer) {
            depthStencil.image.width = w;
            depthStencil.image.height = h;
            depthStencil.free();
            depthStencil.init();
            depthStencil.attach();
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
        buffer.init();
        buffer.bind();

        if (buffer.type == GL_TEXTURE_CUBE_MAP) {
            for (int i = 0 ; i < 6 ; i++) {
                buffer.type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
                image.internalFormat = GL_COLOR_ATTACHMENT0 + i;
                image.pixelFormat = GL_COLOR_ATTACHMENT0 + i;
                buffer.store(image);
            }
            buffer.type = GL_TEXTURE_CUBE_MAP;
            buffer.updateParams(params);
        } else {
            buffer.store(image);
            buffer.updateParams(params);
        }
    }

    void ColorAttachment::free() {
        buffer.free();
        image.free();
    }

    void ColorAttachment::attach() const {
        if (buffer.type == GL_TEXTURE_CUBE_MAP) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, buffer.id, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, buffer.type, buffer.id, 0);
        }
    }

    void ColorAttachment::resize(int w, int h) {
        image.resize(w, h);
        buffer.store(image);
    }

    void DepthAttachment::init() {
        image.internalFormat = GL_DEPTH_COMPONENT;
        image.pixelFormat = GL_DEPTH_COMPONENT;
        buffer.init();
        buffer.load(image, params);
    }

    void DepthAttachment::free() {
        buffer.free();
        image.free();
    }

    void DepthAttachment::attach() {
        if (buffer.type == GL_TEXTURE_CUBE_MAP) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.id, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, buffer.type, buffer.id, 0);
        }
    }

    void DepthStencilAttachment::init() {
        image.internalFormat = GL_DEPTH24_STENCIL8;
        image.pixelFormat = GL_DEPTH_STENCIL;
        image.pixelType = PixelType::UINT248;
        buffer.init();
        buffer.bind();
        buffer.store(image);
    }

    void DepthStencilAttachment::free() {
        buffer.free();
        image.free();
    }

    void DepthStencilAttachment::attach() {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, buffer.type, buffer.id, 0);
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