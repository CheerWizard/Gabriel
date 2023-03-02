#include <buffers.h>

namespace gl {

    u32 vao_init() {
        u32 id;
        glGenVertexArrays(1, &id);
        return id;
    }

    void vao_bind(u32 vao) {
        glBindVertexArray(vao);
    }

    void vao_free(u32 vao) {
        glDeleteVertexArrays(1, &vao);
    }

    void vbo_bind(u32 vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    void vbo_free(u32 vbo) {
        glDeleteBuffers(1, &vbo);
    }

    void vbo_set_format(const vertex_format& format) {
        u32 attr_count = format.attrs.size();
        size_t attr_offset = 0;
        size_t stride = format.stride;
        for (u32 i = 0; i < attr_count; i++) {
            attr_type attr = format.attrs[i];
            glVertexAttribPointer(
                    i, attr,
                    GL_FLOAT, GL_FALSE,
                    stride, (void *) attr_offset
            );
            glEnableVertexAttribArray(i);
            attr_offset += attr * sizeof(float);
        }
    }

    void ibo_bind(u32 ibo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }

    void ibo_free(u32 ibo) {
        glDeleteBuffers(1, &ibo);
    }

    u32 ibo_init(u32* indices, u32 index_count, int alloc_type) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, alloc_type);

        return id;
    }

    u32 ibo_init(const u32* indices, u32 index_count, int alloc_type) {
        u32 id;
        glGenBuffers(1, &id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, alloc_type);

        return id;
    }

    static void fbo_attach(render_buffer* rbo) {
        // render buffer objects
        if (rbo) {
            glGenRenderbuffers(1, &rbo->id);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo->id);

            if (rbo->samples > 1) {
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, rbo->samples, rbo->format, rbo->width, rbo->height);
            } else {
                glRenderbufferStorage(GL_RENDERBUFFER, rbo->format, rbo->width, rbo->height);
            }

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbo->type, GL_RENDERBUFFER, rbo->id);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    }

    u32 fbo_init(
            std::vector<color_attachment>* colors,
            depth_attachment* depth,
            depth_stencil_attachment* depth_stencil,
            render_buffer* rbo
    ) {
        u32 id;
        glGenFramebuffers(1, &id);

        fbo_bind(id);

        // color attachments
        if (colors) {
            size_t colors_size = colors->size();
            for (int i = 0 ; i < colors_size ; i++) {
                auto& color = colors->operator[](i);
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

                    texture_params_update(texture, params);
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

                    texture_params_update(texture, params);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture.type, texture.id, 0);
                }
            }
        }

        // depth attachment
        if (depth) {
            auto& texture = depth->view;
            const auto& params = depth->params;
            const auto& data = depth->data;

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

                texture_params_update(texture, params);
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

                texture_params_update(texture, params);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.type, texture.id, 0);
            }
        }

        // depth-stencil attachments
        if (depth_stencil) {
            glGenTextures(1, &depth_stencil->id);
            u32 depth_stencil_id = depth_stencil->id;
            u32 depth_stencil_type = depth_stencil->type;
            glBindTexture(depth_stencil_type, depth_stencil_id);

            if (depth_stencil_type == GL_TEXTURE_2D_MULTISAMPLE) {
                glTexImage2DMultisample(
                        depth_stencil_type,
                        depth_stencil->samples,
                        GL_DEPTH24_STENCIL8,
                        depth_stencil->width, depth_stencil->height,
                        GL_TRUE
                );
            } else {
                glTexImage2D(
                        depth_stencil_type, 0,
                        GL_DEPTH24_STENCIL8,
                        depth_stencil->width, depth_stencil->height,
                        0,
                        GL_DEPTH_STENCIL,
                        depth_stencil->primitive_type,
                        depth_stencil->data
                );
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_type, depth_stencil_id, 0);
        }

        fbo_attach(rbo);

        if (!colors || colors->empty()) {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        } else {
            int colors_size = colors->size();
            u32* color_attachments = (u32*) calloc(colors_size, sizeof(u32));
            for (int i = 0 ; i < colors_size ; i++) {
                color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glDrawBuffers(colors_size, color_attachments);
        }

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            print_err("fbo_init(): Failed to complete framebuffer!");
            print_err("Error status=" << status);
        }

        fbo_unbind();

        return id;
    }

    u32 fbo_init(render_buffer& rbo) {
        u32 id;
        glGenFramebuffers(1, &id);
        fbo_bind(id);

        fbo_attach(&rbo);

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            print_err("fbo_init(): Failed to complete framebuffer!");
            print_err("Error status=" << status);
        }

        fbo_unbind();
        return id;
    }

    void fbo_bind(u32 fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void fbo_unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void fbo_free(u32 fbo) {
        glDeleteFramebuffers(1, &fbo);
    }

    void fbo_free_attachment(color_attachment& color) {
        texture_free(color.view.id);
    }

    void fbo_free_attachment(std::vector<color_attachment>& colors) {
        for (auto& color : colors) {
            texture_free(color.view.id);
        }
        colors.clear();
    }

    void fbo_free_attachment(depth_attachment& depth) {
        texture_free(depth.view.id);
    }

    void fbo_free_attachment(depth_stencil_attachment& depth_stencil) {
        texture_free(depth_stencil.id);
    }

    void fbo_free_attachment(render_buffer& rbo) {
        glDeleteRenderbuffers(1, &rbo.id);
    }

    void fbo_blit(
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

    void fbo_update(const render_buffer& rbo) {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo.id);
        glRenderbufferStorage(GL_RENDERBUFFER, rbo.format, rbo.width, rbo.height);
    }

    u32 ubo_init(u32 binding, long long size) {
        u32 ubo;
        glGenBuffers(1, &ubo);

        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, null, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);

        return ubo;
    }

    void ubo_bind(u32 ubo) {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    }

    void ubo_update(u32 ubo, const ubo_data& ubo_data) {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        ubo_update(ubo_data);
    }

    void ubo_update(const ubo_data& ubo_data) {
        glBufferSubData(GL_UNIFORM_BUFFER, ubo_data.offset, ubo_data.size, ubo_data.data);
    }

    void ubo_free(u32 ubo) {
        glDeleteBuffers(1, &ubo);
    }

}