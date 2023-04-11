#include <transparency.h>

namespace gl {

    void TransparentRenderer::init(Shader& shader, int w, int h) {
        this->shader = shader;
        composite_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/oit_composite.frag"
        );

        // setup accumulation
        ColorAttachment scene_accum = { 0, w, h };
        scene_accum.image.internal_format = GL_RGBA16F;
        scene_accum.image.pixel_format = GL_RGBA;
        scene_accum.image.pixel_type = PixelType::FLOAT;
        scene_accum.params.min_filter = GL_LINEAR;
        scene_accum.params.mag_filter= GL_LINEAR;
        // setup alpha revealage
        ColorAttachment scene_reveal = { 1, w, h };
        scene_reveal.image.internal_format = GL_R8;
        scene_reveal.image.pixel_format = GL_RED;
        scene_reveal.image.pixel_type = PixelType::FLOAT;
        scene_reveal.params.min_filter = GL_LINEAR;
        scene_reveal.params.mag_filter= GL_LINEAR;
        fbo.colors = { scene_accum, scene_reveal };
        fbo.init_colors();
        fbo.rbo = { w, h };
        fbo.rbo.init();
        fbo.init();
        fbo.attach_colors();
        fbo.attach_render_buffer();
        fbo.complete();

        // setup composite color
        ColorAttachment composite_color = scene_accum;
        composite_fbo.colors = { composite_color };
        composite_fbo.init_colors();
        composite_fbo.rbo = { w, h };
        composite_fbo.rbo.init();
        composite_fbo.init();
        composite_fbo.attach_colors();
        composite_fbo.attach_render_buffer();
        composite_fbo.complete();

        vao.init();

        render_target = composite_fbo.colors[0].buffer;
        transparent_buffer = {
                fbo.colors[0].buffer,
                fbo.colors[1].buffer,
        };
    }

    void TransparentRenderer::free() {
        composite_shader.free();
        fbo.free();
        composite_fbo.free();
        vao.free();
    }

    void TransparentRenderer::resize(int w, int h) {
        fbo.resize(w, h);
        composite_fbo.resize(w, h);
    }

    void TransparentRenderer::begin() {
        fbo.bind();
        glDepthMask(GL_FALSE);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        static glm::vec4 COLOR_ZERO = glm::vec4(0);
        static glm::vec4 COLOR_ONE = glm::vec4(1);
        glClearBufferfv(GL_COLOR, 0, &COLOR_ZERO[0]);
        glClearBufferfv(GL_COLOR, 1, &COLOR_ONE[0]);

        shader.use();
    }

    void TransparentRenderer::end() {
        composite_fbo.bind();
        glDepthFunc(GL_ALWAYS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        composite_shader.use();
        composite_shader.bind_sampler("accum", 0, fbo.colors[0].buffer);
        composite_shader.bind_sampler("reveal", 1, fbo.colors[1].buffer);
        vao.draw_quad();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void TransparentRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable) {
        shader.set_uniform_args("object_id", entity_id);
        transform.update(shader);
        drawable.draw();
    }

    void TransparentRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material) {
        shader.set_uniform_args("object_id", entity_id);
        transform.update(shader);
        material.update(shader, 0);
        drawable.draw();
    }

}