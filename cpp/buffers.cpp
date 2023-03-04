#include <buffers.h>

namespace gl {

    void VertexArray::init() {
        glGenVertexArrays(1, &id);
    }

    void VertexArray::free() {
        glDeleteVertexArrays(1, &id);
    }

    void VertexArray::bind() const {
        glBindVertexArray(id);
    }

    void VertexArray::draw_quad() const {
        glBindVertexArray(id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void VertexBuffer::init() {
        glGenBuffers(1, &id);
    }

    void VertexBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void VertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void VertexBuffer::set_format(const VertexFormat &format) {
        u32 attr_count = format.attrs.size();
        size_t attr_offset = 0;
        size_t stride = format.stride;
        for (u32 i = 0; i < attr_count; i++) {
            AttrType attr = format.attrs[i];
            glVertexAttribPointer(
                    i, attr,
                    GL_FLOAT, GL_FALSE,
                    stride, (void *) attr_offset
            );
            glEnableVertexAttribArray(i);
            attr_offset += attr * sizeof(float);
        }
    }

    void IndexBuffer::init(u32 *indices, u32 index_count, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, alloc_type);
    }

    void IndexBuffer::init(const u32 *indices, u32 index_count, int alloc_type) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, alloc_type);
    }

    void IndexBuffer::free() {
        glDeleteBuffers(1, &id);
    }

    void IndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void IndexBuffer::update(u32* indices, u32 index_count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * index_count, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::update(const u32* indices, u32 index_count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(u32) * index_count, indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void FrameBuffer::init() {
        glGenFramebuffers(1, &id);

        bind();

        // color attachments
        if (flags & FrameBufferFlags::init_colors) {
            attach_colors();
        }

        // depth attachment
        if (flags & FrameBufferFlags::init_depth) {
            attach_depth();
        }

        // depth-stencil attachment
        if (flags & FrameBufferFlags::init_depth_stencil) {
            attach_depth_stencil();
        }

        // render buffer attachment
        if (flags & FrameBufferFlags::init_render_buffer) {
            attach_render_buffer();
        }

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

    void FrameBuffer::attach_colors() {
        size_t colors_size = colors.size();
        for (int i = 0 ; i < colors_size ; i++) {
            auto& color = colors.operator[](i);
            auto& texture = color.view;
            const auto& params = color.params;
            const auto& data = color.data;

            glGenTextures(1, &texture.id);
            glBindTexture(texture.type, texture.id);

            if (texture.type == GL_TEXTURE_CUBE_MAP) {
                for (int j = 0 ; j < 6 ; j++) {
                    glTexImage2D(
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + j,
                            0,
                            GL_COLOR_ATTACHMENT0 + i,
                            data.width, data.height,
                            0,
                            GL_COLOR_ATTACHMENT0 + i,
                            data.primitive_type,
                            data.data
                    );
                }

                texture.update_params(params);
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.id, 0);
            }
            else {
                if (texture.type == GL_TEXTURE_2D_MULTISAMPLE) {
                    glTexImage2DMultisample(
                            texture.type,
                            data.samples,
                            data.internal_format,
                            data.width, data.height,
                            GL_TRUE
                    );
                } else {
                    glTexImage2D(
                            texture.type, 0,
                            data.internal_format,
                            data.width, data.height,
                            0,
                            data.data_format,
                            data.primitive_type,
                            data.data
                    );
                }

                texture.update_params(params);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.type, texture.id, 0);
            }
        }
    }

    void FrameBuffer::attach_depth() {
        auto& texture = depth.view;
        const auto& params = depth.params;
        const auto& data = depth.data;

        glGenTextures(1, &texture.id);
        glBindTexture(texture.type, texture.id);

        if (texture.type == GL_TEXTURE_CUBE_MAP) {
            for (int i = 0 ; i < 6 ; i++) {
                glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0,
                        GL_DEPTH_COMPONENT,
                        data.width, data.height,
                        0,
                        GL_DEPTH_COMPONENT,
                        data.primitive_type,
                        data.data
                );
            }

            texture.update_params(params);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.id, 0);
        }
        else {
            if (texture.type == GL_TEXTURE_2D_MULTISAMPLE) {
                glTexImage2DMultisample(
                        texture.type,
                        data.samples,
                        GL_DEPTH_COMPONENT,
                        data.width, data.height,
                        GL_TRUE
                );
            } else {
                glTexImage2D(
                        texture.type,
                        0,
                        GL_DEPTH_COMPONENT,
                        data.width, data.height,
                        0,
                        GL_DEPTH_COMPONENT,
                        data.primitive_type,
                        data.data
                );
            }

            texture.update_params(params);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.type, texture.id, 0);
        }
    }

    void FrameBuffer::attach_depth_stencil() {
        glGenTextures(1, &depth_stencil.view.id);
        u32 depth_stencil_id = depth_stencil.view.id;
        u32 depth_stencil_type = depth_stencil.view.type;
        glBindTexture(depth_stencil_type, depth_stencil_id);

        if (depth_stencil_type == GL_TEXTURE_2D_MULTISAMPLE) {
            glTexImage2DMultisample(
                    depth_stencil_type,
                    depth_stencil.samples,
                    GL_DEPTH24_STENCIL8,
                    depth_stencil.width, depth_stencil.height,
                    GL_TRUE
            );
        } else {
            glTexImage2D(
                    depth_stencil_type, 0,
                    GL_DEPTH24_STENCIL8,
                    depth_stencil.width, depth_stencil.height,
                    0,
                    GL_DEPTH_STENCIL,
                    depth_stencil.primitive_type,
                    depth_stencil.data
            );
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_type, depth_stencil_id, 0);

    }

    void FrameBuffer::attach_render_buffer() {
        glGenRenderbuffers(1, &rbo.id);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo.id);

        if (rbo.samples > 1) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, rbo.samples, rbo.format, rbo.width, rbo.height);
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, rbo.format, rbo.width, rbo.height);
        }

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbo.type, GL_RENDERBUFFER, rbo.id);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void FrameBuffer::init_with_render_buffer() {
        glGenFramebuffers(1, &id);
        bind();

        attach_render_buffer();

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
        glDeleteFramebuffers(1, &id);

        if (flags & FrameBufferFlags::init_colors) {
            free_colors();
        }

        if (flags & FrameBufferFlags::init_depth) {
            depth.free();
        }

        if (flags & FrameBufferFlags::init_depth_stencil) {
            depth_stencil.free();
        }

        if (flags & FrameBufferFlags::init_render_buffer) {
            rbo.free();
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
        if (buffers_size <= 1) {
            glBlitFramebuffer(0, 0, in_w, in_h, 0, 0, out_w, out_h, buffer_bit, filter);
        } else {
            u32* buffers = (u32*) calloc(buffers_size, sizeof(u32));
            for (int i = 0 ; i < buffers_size ; i++) {
                buffers[i] = GL_COLOR_ATTACHMENT0 + i;
                glReadBuffer(buffers[i]);
                glDrawBuffer(buffers[i]);
                glBlitFramebuffer(0, 0, in_w, in_h, 0, 0, out_w, out_h, buffer_bit, filter);
            }
            glDrawBuffers(buffers_size, buffers);
        }
    }

    void FrameBuffer::resize(int w, int h) {
        for (auto& color : colors) {
            color.data.width = w;
            color.data.height = h;
        }

        depth.data.width = w;
        depth.data.height = h;

        depth_stencil.width = w;
        depth_stencil.height = h;

        rbo.width = w;
        rbo.height = h;

        free();
        glViewport(0, 0, w, h);
        init();
    }

    void ColorAttachment::free() {
        view.free();
    }

    void DepthAttachment::free() {
        view.free();
    }

    void DepthStencilAttachment::free() {
        view.free();
    }

    void RenderBuffer::free() {
        glDeleteRenderbuffers(1, &id);
    }

    void RenderBuffer::update() {
        glBindRenderbuffer(GL_RENDERBUFFER, id);
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    }

    void UniformBuffer::init(u32 binding, long long size) {
        glGenBuffers(1, &id);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, size, null, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    }

    void UniformBuffer::bind() {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void UniformBuffer::update(const UniformData& ubo_data) {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, ubo_data.offset, ubo_data.size, ubo_data.data);
    }

    void UniformBuffer::free() {
        glDeleteBuffers(1, &id);
    }

}